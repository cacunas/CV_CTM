#include "HRSegmentModule.h"

using namespace std;
using namespace cv;

HRSegmentModule::HRSegmentModule(Datapool* i_data) :
    ModuleInterface(i_data) {
    this->firstTime = true;
    this->alpha = 0.1;
    this->beta = 0.75;
}

HRSegmentModule::~HRSegmentModule()
{

}

bool HRSegmentModule::setParameters(QDomNode& config)
{
    if(config.isNull()) {
        return true;
    }

    return true;
}

bool HRSegmentModule::init()
{
    return true;
}

bool HRSegmentModule::run()
{
    if(this->firstTime)
    {

        this->firstTime = false;
        if(m_data->bgImage == NULL)
        {
            m_data->bgImage=
                    new QImage(m_data->currentImage->width(),
                               m_data->currentImage->height(),
                               QImage::Format_RGB888); //Set Background
        }
        if(m_data->grayImage == NULL)
        {
            m_data->grayImage=
                    new QImage(m_data->currentImage->width(),
                               m_data->currentImage->height(),
                               QImage::Format_Indexed8); //Set Background
        }
        if(m_data->persoImage == NULL)
        {
            m_data->persoImage=
                    new QImage(m_data->currentImage->width(),
                               m_data->currentImage->height(),
                               QImage::Format_RGB888); //Set Background
        }
        if(m_data->fgImage == NULL)
        {
            m_data->fgImage=
                    new QImage(m_data->currentImage->width(),
                               m_data->currentImage->height(),
                               QImage::Format_RGB888); //Set Background
        }

    }

    this->GrassClassifier();
    this->Line_detect();

    this->DLowIntGrad(m_data->currentImage, m_data->persoImage);

    m_data->fgImage = this->ForeGround(m_data->bgImage);
    //this->HackForeGround();

    return true;
}

bool HRSegmentModule::updateParameters()
{
    return true;
}

vector<hist> HRSegmentModule::calculateHistograms(QImage *img)
{
    vector<hist> channels(4);

    for (int i=0; i<4; i++)
        channels[i] = hist(256,0);

    QRgb pixel;

    for (int x=0; x<img->width(); x++) {
        for (int y=0; y<img->height(); y++) {
            pixel = img->pixel(x,y);
            channels[0][qRed(pixel)]++;
            channels[1][qGreen(pixel)]++;
            channels[2][qBlue(pixel)]++;
            channels[3][qGray(pixel)]++;
        }
    }

    return channels;
}

vector<float> HRSegmentModule::calculateMoments(vector<hist> channels) {
    vector<float> fMoments(4,0.);
    vector<float> sMoments(4,0.);

    float num_u=0, num_v=0, den=0;

    for (int ch=0; ch<4; ch++) {
        for (int i=0; i<256; i++) {
            if ( channels[ch][i] >= (alpha * channels[ch][cvRound(A_p[ch])]) )
            {
                num_u += i*channels[ch][i];
                num_v += i*i*channels[ch][i];
                den += channels[ch][i];
            }
        }

        fMoments[ch] = num_u/den;
        sMoments[ch] = num_v/den;
        num_u = num_v = den = 0;
    }

    vector<float> moments = fMoments;
    moments.insert(moments.end(),sMoments.begin(),sMoments.end());

    return moments;
}

void HRSegmentModule::calculatePeaks(vector<hist> channels)
{
    vector<float> peaks(4,0);

    if (firstTime)
    {
        // Compute intensity peaks
        int max[4] = {
            channels[0][0],
            channels[1][0],
            channels[2][0],
            channels[3][0]};

        for (int ch=0; ch<4; ch++) {
            for (int i=0; i<256; i++) {
                if (channels[ch][i] > max[ch]) {
                    max[ch] = channels[ch][i];
                    peaks[ch] = (float) i;
                }
            }
        }
    }

    else
    {
        vector<float> aux = this->calculateMoments(channels);
        for (int ch=0; ch<4; ch++)
            peaks[ch] = aux[ch];
    }

    for (int ch=0; ch<4; ch++)
        A_p[ch] = peaks[ch];
}

void HRSegmentModule::calculateThresholds(vector<hist> ch) {
    vector<float> moments = this->calculateMoments(ch);

    for (int ch=0; ch<3; ch++) {
        A_t[ch] = sqrt(moments[ch+4]-moments[ch]*moments[ch]);
    }

    A_t[3] = A_p[3] + beta * sqrt(moments[7]-moments[3]*moments[3]);
}

void HRSegmentModule::GrassClassifier()
{
    QImage *image = m_data->currentImage;

    /* Histograms for each channel:
     * 0 - red
     * 1 - green
     * 2 - blue
     * 3 - gray
     */
    vector<hist> channels = this->calculateHistograms(image);

    // Compute channels' peaks
    // red:0, blue:1, green:2, gray:3
    this->calculatePeaks(channels);

    // Compute thresholds
    this->calculateThresholds(channels);

    // Stores current pixel value when reading the image
    QRgb pixel;

    for (int x=0; x < image->width(); x++) {
        for (int y=0; y < image->height(); y++) {
            pixel = image->pixel(x,y);
            if (
                    qGreen(pixel) > qRed(pixel) &&
                    qGreen(pixel) > qBlue(pixel) &&
                    abs(qRed(pixel) - A_p[0]) < A_t[0] &&
                    abs(qGreen(pixel) - A_p[1]) < A_t[1] &&
                    abs(qBlue(pixel) - A_p[2]) < A_t[2] &&
                    qGray(pixel) < A_t[3]
                    )

                m_data->bgImage->setPixel(x,y,m_data->currentImage->pixel(x,y));
            else
                m_data->bgImage->setPixel(x,y,0);
        }
    }
}


void HRSegmentModule::Line_detect()
{
    if(m_data->currentImage == NULL)
    {
        AppendToLog("SegemntationModule: Warning: No current image. Aborting execution...\n");
    }
    if(m_data->grayImage == NULL)
    {
        AppendToLog("SegemntationModule: Warning: No Background image. Aborting execution...\n");
    }

    //AppendToLog("asd1");

    QImage *current_img = this->m_data->currentImage;
    QImage *gray_img = this->m_data->grayImage;
    QImage *bg_img = this->m_data->bgImage;
    int current_bpl = current_img->bytesPerLine();
    int gray_bpl = gray_img->bytesPerLine();
    int bg_bpl = bg_img->bytesPerLine();
    int w = gray_img->width();
    int h = gray_img->height();
    uchar *current_bits = current_img->bits();
    uchar *gray_bits = gray_img->bits();

    for(int y=0; y<h; y++)
    {
        for(int x=0; x<w; x++)
        {
            int current_pos = y*current_bpl + 4*x;
            int gray_pos = y*gray_bpl + x;
            float promedio = float(current_bits[current_pos])
                    + float(current_bits[current_pos+1])
                    + float(current_bits[current_pos+2]);
            promedio = promedio/3;
            uchar gray  = (char) promedio;
            gray_bits[gray_pos] = gray;
        }
    }

    this->ApplyFilter(gray_img,gray_img);

}


void HRSegmentModule::ApplyFilter(QImage *f_in, QImage *f_out)
{
    int w = f_in->width(), h = f_in->height();

    cv::Mat kernel = (Mat_<double>(3,3) << -2, 1, -2, 1, 4, 1, -2, 1, -2);
    cv::Mat f(h, w, CV_8UC1);

    int bl = f_in->bytesPerLine();
    uchar *fin_p = f_in->bits();
    uchar *fout_p = f_out->bits();

    memcpy(f.data, fin_p, h*bl);

    filter2D(f, f, -1 , kernel, Point( -1, -1 ), 0, BORDER_DEFAULT );

    memcpy(fout_p, f.data, h*bl);
}

void HRSegmentModule::DLowIntGrad(QImage* src, QImage *dst)
{
    if (src == NULL)
    {
        cout << "Error: src NULL\n in DLowIntGrad(src,dst)\n";
        return;
    }

    // free dst memory first
    delete dst;

    // Stores current pixel value when reading the image
    QRgb pixel;

    dst = new QImage(*src);

    //cout << "DEBUG:\t GL_t " << A_t[3] << endl;

    for (int x=0; x < src->width(); x++) {
        for (int y=0; y < src->height(); y++) {
            pixel = src->pixel(x,y);

            //cout << "DEBUG:\t qGray(pixel) " << qGray(pixel) << endl;

            if (
                    qRed(pixel)     >= (A_p[0] + .5*A_t[0]) &&
                    qGreen(pixel)   >= (A_p[1] + .5*A_t[1]) &&
                    qBlue(pixel)    >= (A_p[2] + .5*A_t[2]) //&&
                    //qGray(pixel)    >= A_t[3]
                    )

                dst->setPixel(x,y,src->pixel(x,y));
            else
                dst->setPixel(x,y,qRgb(0,0,0));
        }
    }
}

void HRSegmentModule::HackForeGround()
{
    /* This module discard pixels outside the ROI, 'cause the example is dirty */
    QImage* img = m_data->fgImage;
    QRgb pixel;

    for (int x=0; x < img->width(); x++) {
        for (int y=0; y < img->height(); y++) {
            pixel = img->pixel(x,y);

            if (!(
                    //                    (y-84) <= (((202-84)/(257-28))*(x-28)) && // L1-2
                    //                    (y-202) <= (((110-202)/(524-257))*(x-257)) && // L2-3
                    //                    (y-110) >= (((45-110)/(218-524))*(x-524)) && // L3-4
                    //                    (y-45) >= (((84-45)/(28-218))*(x-218)) // L4-1
                    ((float)y > 70 + 0.5*(float)x) &&
                    ((float)y < 85 - 0.2*(float)x) &&
                    ((float)y <  -5 + 0.22*(float)x) &&
                    ((float)y >  275 - 0.32*(float)x)
                    ))
            {
                img->setPixel(x,y,pixel);
            } else {
                img->setPixel(x,y,qGray(0));
            }
        }
    }
}

QImage* HRSegmentModule::ForeGround(QImage* img)
{
    /* First, we apply a 3x3 Median Filter to non-grass areas (including players) */
    //QImage* img = new QImage(*img_i);

    QRgb pixel;

    for (int x=0; x < img->width(); x++) {
        for (int y=0; y < img->height(); y++) {
            pixel = img->pixel(x,y);

            if (pixel == qRgb(0,0,0)) {
                img->setPixel(x,y,m_data->currentImage->pixel(x,y));
            }  else {
                img->setPixel(x,y,qRgb(0,0,0));
            }
        }
    }

    Mat src = ASM::QImageToCvMat(*img);
    Mat dst;

    // Apply median filter
    medianBlur ( src, dst, 3 );

    // gray version
    Mat grayIm (dst.size(), CV_8U);

    // first convert the image to grayscale
    cvtColor (dst, grayIm, CV_RGB2GRAY);

    // Binary mask
    Mat binMask (grayIm.size(), grayIm.type());

    // then adjust the threshold to actually make it binary
    threshold(grayIm, binMask, 100, 255, CV_THRESH_BINARY);


    // Apply aperture
    Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
    morphologyEx(binMask, binMask, cv::MORPH_OPEN, element);

    morphologyEx(binMask, binMask, cv::MORPH_CLOSE, element);

    //delete img;
    img = new QImage(ASM::cvMatToQImage(binMask));

    return img;
}

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
    QImage* currIm = m_data->currentImage;
    QImage* grayIm = m_data->grayImage;
    QImage* bgIm   = m_data->bgImage;
    QImage* fgIm   = m_data->fgImage;
    QImage* lines  = m_data->persoImage;
    QImage* grass; /* Actually BackGround */

    *currIm = currIm->convertToFormat(QImage::Format_RGB888);


//    cout << "DEBUG: currIm->format()=" << currIm->format() << endl;
//    cout << QImage::Format_ARGB32 << endl;
//    cout << QImage::Format_ARGB32_Premultiplied << endl;
//    cout << QImage::Format_ARGB4444_Premultiplied << endl;
//    cout << QImage::Format_ARGB6666_Premultiplied << endl;
//    cout << QImage::Format_ARGB8555_Premultiplied << endl;
//    cout << QImage::Format_ARGB8565_Premultiplied << endl;
//    cout << QImage::Format_Indexed8 << endl;
//    cout << QImage::Format_Invalid << endl;
//    cout << QImage::Format_Mono << endl;
//    cout << QImage::Format_MonoLSB << endl;
//    cout << QImage::Format_RGB16 << endl;
//    cout << QImage::Format_RGB32 << endl;
//    cout << QImage::Format_RGB444 << endl;
//    cout << QImage::Format_RGB555 << endl;
//    cout << QImage::Format_RGB666 << endl;
//    cout << QImage::Format_RGB888 << endl;




    if(this->firstTime)
    {
        this->firstTime = false;

        int w = currIm->width();
        int h = currIm->height();

        if(bgIm == NULL)
            bgIm = new QImage(w, h, QImage::Format_RGB888);

        if(grayIm == NULL)
            grayIm= new QImage(w, h, QImage::Format_Indexed8);

        if(lines == NULL)
            lines = new QImage(w, h, QImage::Format_RGB888);

        if(fgIm == NULL)
            fgIm = new QImage(w, h, QImage::Format_RGB888);
    }


    this->GrassClassifier(*currIm, *grass);

    bgIm = grass;

    this->Line_detect(*currIm, *lines);
    this->DLowIntGrad(*currIm, *lines);

    this->ForeGround(*currIm, *bgIm, *fgIm);
    //this->HackForeGround();

    return true;
}

bool HRSegmentModule::updateParameters()
{
    return true;
}

vector<hist> HRSegmentModule::calculateHistograms(const QImage& img)
{
    vector<hist> channels(4);

    for (int i=0; i<4; i++)
        channels[i] = hist(256,0);

    QRgb pixel;

    for (int x=0; x<img.width(); x++) {
        for (int y=0; y<img.height(); y++) {
            pixel = img.pixel(x,y);
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
        int max[4] =
        {
            channels[0][0],
            channels[1][0],
            channels[2][0],
            channels[3][0]
        };

        for (int ch=0; ch<4; ch++)
        {
            for (int i=0; i<256; i++)
            {
                if (channels[ch][i] > max[ch])
                {
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

    for (int ch=0; ch<3; ch++)
        A_t[ch] = sqrt(moments[ch+4]-moments[ch]*moments[ch]);

    A_t[3] = A_p[3] + beta * sqrt(moments[7]-moments[3]*moments[3]);
}

/*
 * @currIm : Frame image
 * @grass : (empty) QImage. Stor image with only grass pixels
 */
void HRSegmentModule::GrassClassifier(const QImage& currIm, QImage& grass)
{
    // Copy on temporary memory, for safety
    QImage myCurrIm = QImage(currIm);
    int w = myCurrIm.width();
    int h = myCurrIm.height();
    delete &grass;
    grass = QImage(w, h, myCurrIm.format());

    /* Histograms for each channel:
     * 0 - red
     * 1 - green
     * 2 - blue
     * 3 - gray
     */
    vector<hist> channels = this->calculateHistograms(myCurrIm);

    // Compute channels' peaks
    // red:0, blue:1, green:2, gray:3
    this->calculatePeaks(channels);

    // Compute thresholds
    this->calculateThresholds(channels);

    // Stores current pixel value when reading the image
    QRgb pixel;

    for (int x=0; x < w; x++)
    {
        for (int y=0; y < h; y++)
        {
            pixel = myCurrIm.pixel(x,y);
            bool condition =
                    qGreen(pixel) > qRed(pixel) &&
                    qGreen(pixel) > qBlue(pixel) &&
                    abs(qRed(pixel) - A_p[0]) < A_t[0] &&
                    abs(qGreen(pixel) - A_p[1]) < A_t[1] &&
                    abs(qBlue(pixel) - A_p[2]) < A_t[2] &&
                    qGray(pixel) < A_t[3];

            if (condition)
                grass.setPixel(x,y,pixel);
            else
                grass.setPixel(x,y,qRgb(0,0,0));
        }
    }
}

void HRSegmentModule::Line_detect(const QImage& currIm, QImage& lineIm)
{
    // Initialized as gray image format
    lineIm = QImage(currIm.convertToFormat(QImage::Format_Indexed8));

    this->ApplyFilter(lineIm, lineIm);
}

void HRSegmentModule::ApplyFilter(const QImage& f_in, QImage& f_out)
{
    cout << "DEBUG:\t f_in.format() " << f_in.format() << endl;
    cout << "DEBUG:\t Format_Indexed8 " << QImage::Format_Indexed8 << endl;

    //int w = f_in->width();
    //int h = f_in->height();

    cv::Mat kernel = (Mat_<double>(3,3) << -2, 1, -2, 1, 4, 1, -2, 1, -2);
    //cv::Mat f(h, w, CV_8UC1);

    Mat f = ASM::QImageToCvMat(f_in);

    //memcpy(f.data, fin_p, h*bl);

    filter2D(f, f, -1 , kernel, Point( -1, -1 ), 0, BORDER_DEFAULT );

    f_out = ASM::cvMatToQImage(f);

    //memcpy(fout_p, f.data, h*bl);
}

void HRSegmentModule::DLowIntGrad(const QImage& src, QImage& dst)
{
    if (src.isNull())
    {
        AppendToLog("Error: src NULL\n in DLowIntGrad(src,dst)\n");
        return;
    } else if (src.format() != QImage::Format_Indexed8) {
        AppendToLog("Error: src not gray\n");
        return;
    }

    // Stores current pixel value when reading the image
    QRgb pixel;

    QImage mySrc = QImage(src);
    dst = QImage(mySrc.size(), mySrc.format());

    for (int x=0; x < mySrc.width(); x++)
    {
        for (int y=0; y < mySrc.height(); y++)
        {
            pixel = mySrc.pixel(x,y);

            bool condition =
                    qRed(pixel)     >= (A_p[0] + .5*A_t[0]) &&
                    qGreen(pixel)   >= (A_p[1] + .5*A_t[1]) &&
                    qBlue(pixel)    >= (A_p[2] + .5*A_t[2]) &&
                    qGray(pixel)    >= A_t[3];

            if (condition)
                dst.setPixel(x,y,pixel);
            else
                dst.setPixel(x,y,qRgb(0,0,0));
        }
    }
}

void HRSegmentModule::HackForeGround()
{
    /* This module discard pixels outside the ROI, 'cause the example is dirty */
    QImage* img = m_data->fgImage;
    QRgb pixel;

    for (int x=0; x < img->width(); x++)
    {
        for (int y=0; y < img->height(); y++)
        {
            pixel = img->pixel(x,y);

            bool condition =
//                    (y-84) <= (((202-84)/(257-28))*(x-28)) && // L1-2
//                    (y-202) <= (((110-202)/(524-257))*(x-257)) && // L2-3
//                    (y-110) >= (((45-110)/(218-524))*(x-524)) && // L3-4
//                    (y-45) >= (((84-45)/(28-218))*(x-218)) // L4-1
                    ((float)y > 70 + 0.5*(float)x) &&
                    ((float)y < 85 - 0.2*(float)x) &&
                    ((float)y <  -5 + 0.22*(float)x) &&
                    ((float)y >  275 - 0.32*(float)x);

            if (!condition)
                img->setPixel(x,y,pixel);
            else
                img->setPixel(x,y,qGray(0));
        }
    }
}

void HRSegmentModule::ForeGround(const QImage& curr, const QImage& bg, QImage& fg)
{
    if (curr.isNull())
    {
        AppendToLog("Error:\t at ForeGround(curr, bg, fg)\n curr NULL\n");
        return;
    }

    /* First, we apply a 3x3 Median Filter to non-grass areas (including players) */
    //QImage* img = new QImage(*img_i);
    QImage myIm = QImage(curr);

    QRgb pixelIm, pixelBg;

    for (int x=0; x < myIm.width(); x++) {
        for (int y=0; y < myIm.height(); y++) {
            pixelIm = myIm.pixel(x,y);
            pixelBg = bg.pixel(x,y);

            if (pixelBg == qRgb(0,0,0)) {
                myIm.setPixel(x,y,pixelIm);
            }  else {
                myIm.setPixel(x,y,qRgb(0,0,0));
            }
        }
    }

    Mat src = ASM::QImageToCvMat(myIm);
    Mat dst;

    // Apply median filter
    medianBlur ( src, dst, 3 );

    // gray version
    Mat grayIm (dst.size(), CV_8UC1);

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
    fg = QImage(ASM::cvMatToQImage(binMask));
}

#include "HMTrackingModule.h"

using namespace std;
using namespace cv;

HMTrackingModule::HMTrackingModule(Datapool* i_data) :
    ModuleInterface(i_data) {
    this->firstTime = true;
    this->alpha = 0.1;
    this->beta = 0.75;
}

HMTrackingModule::~HMTrackingModule()
{

}

bool HMTrackingModule::setParameters(QDomNode& config)
{
    if(config.isNull()) {
        return true;
    }

    return true;
}

bool HMTrackingModule::init()
{
    return true;
}

bool HMTrackingModule::run()
{
    // Current frame from the datapool
    QImage currIm = m_data->currentImage->convertToFormat(QImage::Format_RGB888);
    // To avoid problems of formats, converto to RGB888
    //currIm = currIm.convertToFormat(QImage::Format_RGB888);
    this->HackROI(currIm);

    // Local variables to store results
    QImage grass    = this->GrassClassifier(currIm);
    QImage lines    = this->Line_detect(ROI_mask);
    QImage better   = this->DLowIntGrad(ROI_mask);
    QImage fgIm     = this->ForeGround(ROI_mask, grass);

    if(this->firstTime)
    {
        this->firstTime = false;
    } else {
        delete m_data->bgImage;
        delete m_data->fgImage;
        delete m_data->persoImage;
        delete m_data->grayImage;
    }

    // Copy local results to new memory space, so is not deleted between frames
    m_data->bgImage = new QImage(grass);
    m_data->fgImage = new QImage(fgIm);
    m_data->persoImage = new QImage(better);
    m_data->grayImage = new QImage(lines);

    return true;
}

bool HMTrackingModule::updateParameters()
{
    return true;
}

cv::Mat HMTrackingModule::QImageTocvMat(QImage *img, int format)
{
    return cv::Mat(
                    img->height(),
                    img->width(),
                    format,
                    img->bits(),
                    img->bytesPerLine()
                ).clone();
}

QImage HMTrackingModule::cvMatToQImage(const Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, format);
}

/**
 * @brief HMTrackingModule::calculateHistograms: Calculates Intensity histograms
 * of an image (1 per channel), and stores it on a std::vector.
 * @param img: Input Image (format: QImage::Format_RGB888)
 * @return: std::vector< std::vector<int> > with the intensity histograms
 * (4 histograms: red, blue, green and gray, each with 256 bins, where a bin is
 * a given intensity).
 */
vector<hist> HMTrackingModule::calculateHistograms(const QImage img)
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

/**
 * @brief HMTrackingModule::calculateMoments: Calculates first and second
 * statistical moments of the intensity histograms, and return a std::vector
 * with those moments. Is used for the calculation of peaks and thresholds for
 * the grass classifier.
 * @param channels: Intensity histograms (1 per channel).
 * @return: return a std:vector with the first and second statistical moments
 * for each channel (in total, 8 values).
 */
vector<float> HMTrackingModule::calculateMoments(vector<hist> channels) {
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

/**
 * @brief HMTrackingModule::calculatePeaks: Calculate peak intensity for each
 * channel, and stores those peaks int the private member A_p
 * @param channels: Intensity histograms (1 per channel).
 */
void HMTrackingModule::calculatePeaks(vector<hist> channels)
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

/**
 * @brief HMTrackingModule::calculateThresholds: Calculate thresholds for grass
 * clasifier
 * @param ch: std::vector< std::vector<int> > ch, contains histograms (1 per
 * channel) of intensity (from frame). Is used to calculate the thresholds
 * (stored) int the private member A_t
 */
void HMTrackingModule::calculateThresholds(vector<hist> ch) {
    vector<float> moments = this->calculateMoments(ch);

    for (int ch=0; ch<3; ch++)
        A_t[ch] = sqrt(moments[ch+4]-moments[ch]*moments[ch]);

    A_t[3] = A_p[3] + beta * sqrt(moments[7]-moments[3]*moments[3]);
}

/**
 * @brief HMTrackingModule::GrassClassifier: Takes current frame as input, and
 * returns an image that contains only pixels correspondet to field (grass)
 * pixels
 * @param currIm: Current frame image (format: QImage::Format_RGB888)
 * @return: QImage. Stor image with only grass pixels
 */
QImage HMTrackingModule::GrassClassifier(const QImage currIm)
{
    // Copy on temporary memory, for safety
    QImage myCurrIm = QImage(currIm);
    int w = myCurrIm.width();
    int h = myCurrIm.height();

    QImage grass(w, h, QImage::Format_RGB888);

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

    return grass;
}

/**
 * @brief HMTrackingModule::Line_detect Detects field lines over the present
 * frame
 * @param currIm: current frame (in RGB888 format)
 * @return: return gray image of posible lines (format Index8)
 */
QImage HMTrackingModule::Line_detect(const QImage currIm)
{
    // Initialized as gray image format
    QImage aux = currIm.convertToFormat(QImage::Format_Indexed8);

    return this->ApplyFilter(aux);
}

/**
 * @brief HMTrackingModule::ApplyFilter Appplies custom box filter (normalized)
 * over gray image (Format_Indexed8)
 * @param f_in: input image (format: QImage::Format_Indexed8)
 * @return: returns filtered image, in format QImage::Format_Indexed8
*/
QImage HMTrackingModule::ApplyFilter(const QImage f_in)
{
    QImage f_out = QImage(f_in.size(), f_in.format());

    cv::Mat kernel = (Mat_<double>(3,3) << -2, 1, -2, 1, 4, 1, -2, 1, -2);

    Mat f = ASM::QImageToCvMat(f_in);

    filter2D(f, f, -1 , kernel, Point( -1, -1 ), 0, BORDER_DEFAULT );

    f_out = ASM::cvMatToQImage(f);

    return f_out;
}

/**
 * @brief HMTrackingModule::DLowIntGrad Discards low gradient and low contrast
 * pixels
 * @param src : Input image, in format QImage::Format_RGB888
 * @return: Return image with low gradient and low contrast discarded (in format
 * QImage::Format_RGB888 also
 */
QImage HMTrackingModule::DLowIntGrad(const QImage src)
{
    if (src.isNull())
    {
        AppendToLog("Error: src NULL\n in DLowIntGrad(src)\n");
        exit(-1);
    }

    // Stores current pixel value when reading the image
    QRgb pixel;

    QImage mySrc = QImage(src);
    QImage dst = QImage(mySrc.size(), mySrc.format());

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

    return dst;
}

/**
 * @brief HMTrackingModule::HackROI: Select interest area (example has grass
 * outside field, so is a bad example).
 * @param currIm Current frame, takes it and save the same image but with all
 * pixels outside interest area in black (in this->ROI_mask)
 */
void HMTrackingModule::HackROI(const QImage currIm)
{
    /* This module discard pixels outside the ROI, 'cause the example is dirty */
    ROI_mask = QImage(currIm);
    QRgb pixel;

    int w = ROI_mask.width();
    int h = ROI_mask.height();

    double m_AB = (203.-81.)/(254.-26.);
    double m_BC = (109.-203.)/(524.-254.);
    double m_CD = (45.-109.)/(217.-524.);
    double m_DA = (81.-45.)/(26.-217.);

    for (int x=0; x < w; x++)
    {
        for (int y=0; y < h; y++)
        {
            pixel = ROI_mask.pixel(x,y);

            bool condition =
                    ( (float)(y-81) <= m_AB*(x-26) ) &&
                    ( (float)(y-203) <= m_BC*(x-254) ) &&
                    ( (float)(y-109) >= m_CD*(x-524) ) &&
                    ( (float)(y-45) >= m_DA*(x-217) );

            if (condition)
                ROI_mask.setPixel(x,y,pixel);
            else
                ROI_mask.setPixel(x,y,qRgb(0,0,0));
        }
    }
}

/**
 * @brief HMTrackingModule::ForeGround Calculates Foreground
 * (posible player blobs) using the current frame (in colors) and a background
 * (grass)
 * @param curr: Current frame (format QImage::Format_RGB888)
 * @param bg : Grass area (format QImage::Format_RGB888)
 * @return return binary image for player blob extraction
 * (format: QImage::Format_RGB888)
 */
QImage HMTrackingModule::ForeGround(const QImage curr, const QImage bg)
{
    if (curr.isNull())
    {
        AppendToLog("Error:\t at ForeGround(curr, bg)\n curr NULL\n");
        exit(-1);
    }

    /* First, we apply a 3x3 Median Filter to non-grass areas (including players) */
    QImage myIm = QImage(curr.size(), curr.format());

    QRgb pixelIm, pixelBg;

    for (int x=0; x < curr.width(); x++)
    {
        for (int y=0; y < curr.height(); y++)
        {
            pixelIm = curr.pixel(x,y);
            pixelBg = bg.pixel(x,y);

            if ( (pixelBg == qRgb(0,0,0)) && (pixelIm != qRgb(0,0,0)) )
                myIm.setPixel(x,y,qRgb(255,255,255));
            else
                myIm.setPixel(x,y,qRgb(0,0,0));
        }
    }

    myIm = myIm.convertToFormat(QImage::Format_Indexed8);

    Mat src = this->QImageTocvMat(&myIm, CV_8UC1);
    //Mat dst;

    // Apply median filter
    medianBlur ( src, src, 3 );

    // then adjust the threshold to actually make it binary
    threshold(src, src, 100, 255, CV_THRESH_BINARY);

    // Apply aperture
    Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
    morphologyEx(src, src, cv::MORPH_OPEN, element);

    //myIm = ASM::cvMatToQImage(src);
    myIm = this->cvMatToQImage(src, QImage::Format_Indexed8);
    return myIm;
}

void HMTrackingModule::generateBlobs()
{

}

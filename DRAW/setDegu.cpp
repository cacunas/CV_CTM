#include "setDegu.h"

setDegu::setDegu(Datapool *i_data): drawInterface(i_data){
}


setDegu::~setDegu(){}

//Set module configuration parameters
bool setDegu::setParameters(QDomNode &config){
    /*
    QDomNode n;
    if(config.isNull()) { //Parameter set for module not defined
    } else {
        if( ( n = getParameterNode("afterModule", config) ).isNull() )
            afterModule = '-';
        else
            afterModule = getParameterValue(n);
    }
    */
    return true;
}

//Initialization after reading parameters
bool setDegu::init(){
    return true;
}

//Function executed at each frame
bool setDegu::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setDegu::paint(QImage *image){
    /*
    int w = m_data->fgImage->width(), h = m_data->fgImage->height();

    cv::Mat f(h, w, CV_8UC1);

    uchar  *fg_p = m_data->fgImage->bits();
    int bl = m_data->fgImage->bytesPerLine();

    memcpy(f.data, fg_p, h*bl);


    cv::Mat border_aux(f.size(), CV_8UC1);
    cv::Canny(f,border_aux, 50,100, 3);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat border_copy(border_aux.size(), CV_8UC1);
    border_aux.copyTo(border_copy);
    cv::findContours(border_copy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );


    std::vector<cv::Point> big_contour;
    std::vector<cv::Point> hull;
    if(contours.size() > 0) {
        //Group found contours in one big contour
        for(int i=0;i<contours.size(); i++) {
            if(hierarchy[i][2] < 0) { // No parent, so it's parent
                if(big_contour.empty())
                    big_contour = contours[i];
                else
                    big_contour.insert( big_contour.end(), contours[i].begin(), contours[i].end());
            }
        }
        //Get initial convex hull
        cv::convexHull( big_contour, hull, false );


    } else {
        return true;
    }
    if(hull.size() == 0) {
        return true;
    }


    //cv::erode(aux, aux, element);
    //cv::dilate(aux, aux, element,cv::Point(-1,-1),2);
    //cv::erode(aux, aux, element);

    SpHullModel newHull(new HullModel());

    newHull->local_hull = hull;
    newHull->off_x = 0;
    newHull->off_y = 0;
    //newHull->off_x = j0;
    //newHull->off_y = i0;
    //newHull->id = (*it)->mobile_id;
    newHull->id = NULL;

    int w0 = m_data->fgImage->width(),
        h0 = m_data->fgImage->height();

    //Get principal/minor axis
    std::vector<cv::Point2f> data_aux(h0*w0);
    float mean_x = 0, mean_y = 0;
    int count = 0;



    for(int i=0; i<h0; i++)
        for(int j=0; j<w0; j++)
            if(cv::pointPolygonTest(hull, cv::Point2f(j, i), true) > - 1) {
                data_aux[count++] = cv::Point2f(j, i);
                mean_x += j;
                mean_y += i;
            }
    //data_aux.resize(count);
    //cv::Mat data(2, count, CV_32FC1, &data_aux.front());
    cv::Mat data(2, count, CV_32FC1);
    cv::Point2f x;
    for(int i=0; i<count; i++) {
        data.at<float>(0,i) = data_aux[i].x;
        data.at<float>(1,i) = data_aux[i].y;
    }

    //cv::Mat data();
    mean_x /= count;
    mean_y /= count;
    cv::Mat mean(2, 1, CV_32FC1);
    mean.at<float>(0) = mean_x;
    mean.at<float>(1) = mean_y;

    //2. perform PCA
    cv::PCA pca(data, mean, CV_PCA_DATA_AS_COL, 1);
    //result is contained in pca.eigenvectors (as row vectors)
    //std::cout << pca.eigenvectors << std::endl;

    //3. get angle of principal axis
    float dx = pca.eigenvectors.at<float>(0, 0),
          dy = pca.eigenvectors.at<float>(0, 1),
          scale = 40.0;
    cv::Point3f rline;
    cv::Point2f r1, r2;

    //Get line general form from principal component
    getGeneralLineForm(cv::Point2f(mean_x, mean_y),
                       cv::Point2f(mean_x + dx*scale, mean_y + dy*scale),
                       rline);
    //Get segment from line
    int n1, n2;
    getContourToLineIntersection(hull, rline, r1, r2, &n1, &n2);

    //Get pixel intersections for normals
    std::vector< segment2D<float> > &segs = newHull->segs;
    std::vector< segment2D<float> > &hull_segs = newHull->hull_segs;


    cv::Rect roi;
    roi.x = 0;
    roi.y = 0;
    roi.width = w0;
    roi.height = h0;

    cv::Mat aux(f, roi);

    //Get segments of movement normal to principal axis. Also reorders r1 and r2 in
    //coherence with segments order
    getNormalIntersections(aux, roi, hull, r1, r2, n1, n2, dx, dy, segs, hull_segs);

    newHull->axis1 = r1;
    newHull->axis2 = r2;
    */

    //Set new representation
    //m_data->hulls.push_back(newHull);



    //inactivo temporalmente


    std::map<QString,DeguTrackingModel *>::iterator modeloMain;
    modeloMain= m_data->modelsToFollow.find("main");


    if(modeloMain!=m_data->modelsToFollow.end()){

    }else return true;


    QPainter painter;
    painter.begin(image);








    //if(m_data->degu != NULL && m_data->degu->instances.size() > 0) {
        //SpHullModel h = m_data->degu->instances.back()->hull;
        //SpHullModel hl = newHull;

        SpHullModel hl = modeloMain->second->deguShape;
        if(!hl.isNull()) {
            displayShowHull(painter, hl);
            displayShowAxis(painter, hl);
            displayShowNormals(painter, hl);
            //displayShowHead(painter, m_data->degu, h->off_x, h->off_y);
        }
        //displayShowTrajectory(painter, m_data->degu->trajectory);
    //}
    cv::Point2d head=modeloMain->second->headLocation;
    QPoint headP(head.x,head.y);


    painter.drawEllipse(headP, 20,20);
    painter.end();

    return true;
}

void setDegu::displayShowHull(QPainter &painter, SpHullModel hull) {
    std::vector<cv::Point> &h = hull->local_hull;
    int i, n = h.size();
    if(n == 0)
        return;
    int X   = hull->off_x,
        Y   = hull->off_y;
    cv::Point p1, p2;

    painter.setPen(QColor(0, 255, 0, 255));//green

    p2 = h[n-1];

    for(i = 0; i < n; i++) {
        p1 = p2;
        p2 = h[i];
        painter.drawLine(X  + p1.x, Y + p1.y, X  + p2.x, Y + p2.y);
    }

}

void setDegu::displayShowAxis(QPainter &painter, SpHullModel hull) {
    cv::Point p1 = hull->axis1,
              p2 = hull->axis2;

    if(p1 == p2)
        return;

    painter.setPen(QColor(255, 0, 0, 255));//red

    painter.drawLine(hull->off_x + p1.x, hull->off_y + p1.y,
                     hull->off_x + p2.x, hull->off_y + p2.y);

}


void setDegu::displayShowNormals(QPainter &painter, SpHullModel hull) {
    std::vector< segment2D<float> > &segs = hull->segs;
    int i, n = segs.size();
    if(n == 0)
        return;
    int X   = hull->off_x,
        Y   = hull->off_y;

    painter.setPen(QColor(0, 0, 255, 255));//blue

    for(i = 0; i < n; i++) {
        segment2D<float> &s = segs[i];
        painter.drawLine(X  + s.first.x, Y + s.first.y, X  + s.last.x, Y + s.last.y);
    }

}

void setDegu::displayShowHead(QPainter &painter, DeguModel *d, int offX, int offY) {
    point2D<int> &p = d->headPosition;
    int t = d->insertionType;
    if (t == 0) //Too little, but historic information present
        painter.setPen(QColor(255, 0, 0, 255));//red
    else if (t == 1) //Too little, and no historic information present
        painter.setPen(QColor(0, 255, 0, 255));//green
    else if (t == 2) //Just the body, with no significant tail
        painter.setPen(QColor(0, 0, 255, 255));//blue
    else if (t == 3) //Tail well detected
        painter.setPen(QColor(255, 255, 0, 255));//yellow
    else if (t == 4) //Correction
        painter.setPen(QColor(255, 0, 255, 255));//magenta
    else  //Other
        painter.setPen(QColor(0, 255, 255, 255));//cyan

    painter.drawEllipse(offX-10+p.x, offY-10+p.y, 20, 20);
}

void setDegu::displayShowTrajectory(QPainter &painter, std::deque< point2D<int> > &trajectory) {
    int i, size;
    if( (size = trajectory.size()) == 0 )
        return;

    int x1, y1, x2, y2;
    std::deque< point2D<int> >::reverse_iterator points = trajectory.rbegin();

    //Show first point
    x1 = (int) (*points).x;
    y1 = (int) (*points).y;

    //Oldest white
    painter.setPen(QColor(255,255,255,255)); //white
    painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);

    //Draw next points and lines connecting them
    painter.setPen(QColor(0,0,255,255)); //blue

    for(i=1, points++; i<size ; i++, points++) {
        x2 = x1;
        y2 = y1;

        x1 = (int) (*points).x;
        y1 = (int) (*points).y;

        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);
    }

}


void setDegu::getNormalIntersections(cv::Mat &f,
                                   cv::Rect &roi,
                                   std::vector<cv::Point> &hull,
                                   cv::Point2f &r1, cv::Point2f &r2,
                                   int n1, int n2,
                                   float dx, float dy,
                                   std::vector< segment2D<float> > &segs,
                                   std::vector< segment2D<float> > &hull_segs) {
//                                   std::vector< cv::Point2f > &) {
    cv::Point2f R1, R2;
    int I1, I2, sy;
    //Normalize search: from leftmost point, if equal, higher one.
    if(r1.x < r2.x) {
        R1 = r1;
        R2 = r2;
        I1 = n1;
        sy = (R1.y <= R2.y) ? 1 : -1;
    } else if(r1.x > r2.x) {
        R1 = r2;
        R2 = r1;
        I1 = n2;
        sy = (R1.y <= R2.y) ? 1 : -1;
    } else {
        sy = 1;
        if(r1.y <= r2.y) {
            R1 = r1;
            R2 = r2;
            I1 = n1;
        } else {
            R1 = r2;
            R2 = r1;
            I1 = n2;
        }
    }
    I2 = I1;

    //Set scale and advances in X and Y to move K pixels, given dx and dy.
    float m_K=2;
    float lx = R2.x - R1.x, ly = R2.y - R1.y,
          D_axis = sqrt(lx*lx + ly*ly),
          a = m_K/sqrt(dx*dx + dy*dy),
          Dx = fabs(a*dx),
          Dy = sy*fabs(a*dy);
    int i, n = (int)D_axis/m_K;
    float x, y;

    segs.resize(n);
    hull_segs.resize(n);


    for(i=0, x=R1.x + Dx, y=R1.y + Dy; i<n; i++, x+=Dx, y+=Dy)
        setForegroundSegment(f, roi, hull, segs[i], hull_segs[i], x, y, dx, dy, I1, I2);

    //Reorder principal axis points, w/r to normal segments
    r1 = R1;
    r2 = R2;
}


void setDegu::setForegroundSegment(cv::Mat &f, cv::Rect &roi,
                                                      std::vector<cv::Point> &hull,
                                                      segment2D<float> &seg,
                                                      segment2D<float> &hseg,
                                                      float x, float y,
                                                      float dx, float dy,
                                                      int &I1, int &I2) {
    cv::Point3f rline;
    //Get line form from perpendicular line in x,y
    getGeneralLineForm(x, y, dy, -dx, rline);

    cv::Point2f r1, r2;

//    if(getContourToLineIntersectionIndexed(hull, rline, r1, r2, I1, I2) == 2) {
    if(getContourToLineIntersection(hull, rline, r1, r2) == 2) {
        float xt, yt, Dx = r2.x - r1.x, Dy = r2.y - r1.y, D = sqrt(Dx*Dx + Dy*Dy);
        Dx /= D; Dy /= D; //Unitary increment
        int d, xx, yy, w_mean = 9/2;

        hseg.first.x = r1.x;
        hseg.first.y = r1.y;
        hseg.last.x = r2.x;
        hseg.last.y = r2.y;

        for(xt=r1.x, yt=r1.y, d=0; d<D; xt+=Dx, yt+=Dy, d++) {
            xx = rint(xt); yy = rint(yt);
            if(movementCount(f, 3, yy, xx, roi) > w_mean) {
                seg.first.x = xx;
                seg.first.y = yy;
                break;
            }
        }
        for(xt=r2.x, yt=r2.y, d=0; d<D; xt-=Dx, yt-=Dy, d++) {
            xx = rint(xt); yy = rint(yt);
            if(movementCount(f, 3, yy, xx, roi) > w_mean) {
                seg.last.x = xx;
                seg.last.y = yy;
                break;
            }
        }
    } else
        std::cout << "setForegroundSegment: Error: 2 intersections not found" << std::cout;

}

int setDegu::getContourToLineIntersectionIndexed(std::vector<cv::Point> &polygon,
                                         cv::Point3f &rline,
                                         cv::Point2f &r1, cv::Point2f &r2,
                                         int &n1, int &n2) {

    int i, k1 = 0, k2 = 0, n = polygon.size();
    cv::Point2f p1_1, p2_1, p1_2, p2_2, r;
    bool ready1 = false, ready2 = false, got_same = false;

    p1_1 = polygon[n1];
    p2_1 = polygon[(n1+1 == n ? 0 : n1+1)];
    if(lineSegmentIntersection(rline, p1_1, p2_1, r))
        got_same = true;
    n1 = (n1+1 == n ? 0 : n1+1);
    n2 = (n2-1 == -1) ? n-1 : n2-1;

    do {
        if(!ready1) {
            p1_1 = polygon[n1];
            p2_1 = polygon[(n1+1 == n ? 0 : n1+1)];
            if(lineSegmentIntersection(rline, p1_1, p2_1, r1)) {
                ready1 = true;
                if(got_same) {
                    r2 = r;
                    break;
                }
            } else
                n1 = (n1+1 == n ? 0 : n1+1);
            k1++;
        }
        if(!ready2) {
            p1_2 = polygon[n2];
            p2_2 = polygon[(n2+1 == n ? 0 : n2+1)];
            if(lineSegmentIntersection(rline, p1_2, p2_2, r2)) {
                ready2 = true;
                if(got_same) {
                    r1 = r;
                    break;
                }
            } else
                n2 = (n2-1 == -1) ? n-1 : n2-1;
            k2++;
        }
        if(k1 > n && k2 > n) //If true, there is a problem
            break;

    } while (!ready1 || !ready2);

    if(k1 > n && k2 > n) //If true, there is a problem
        return 0;
    else if(k1 > n || k2 > n) //If true, there is a problem
        return 1;
    return 2;
}

int setDegu::getContourToLineIntersection(std::vector<cv::Point> &polygon,
                                         cv::Point3f &rline,
                                         cv::Point2f &r1, cv::Point2f &r2,
                                         int *n1, int *n2) {
    int i, ind[3], n = polygon.size();
    int num_found = 0;
    cv::Point2f p1, p2, pts[3]; //Max 4 for a convex polygon (2 in 2 corners)
    p2 = polygon[n-1];
    for(i=0; i<n; i++) {
        p1 = p2;
        p2 = polygon[i];
        if(lineSegmentIntersection(rline, p1, p2, pts[num_found])) {
            ind[num_found] = i == 0 ? n-1 : i-1;
            if(++num_found == 3)
                break;
        }
    }

    if (num_found == 1) {
        if(n1 != NULL)
            *n1 = ind[0];
        if(n2 != NULL)
            *n2 = ind[0];
        r1 = pts[0];
        return 1;
    } else if(num_found == 2) {
        r1 = pts[0];
        r2 = pts[1];
        if(n1 != NULL)
            *n1 = ind[0];
        if(n2 != NULL)
            *n2 = ind[1];
        return 2;
    }

    //If more than two, the intersection repeated a polygon point
    if(   fabs(pts[0].y - pts[1].y) < /*EPS*/1e-8
       && fabs(pts[0].x - pts[1].x) < /*EPS*/1e-8) { //Take 1 and 3
        r1 = pts[0];
        r2 = pts[2];
        if(n1 != NULL)
            *n1 = ind[0];
        if(n2 != NULL)
            *n2 = ind[2];

        return 3;
    }

    r1 = pts[0];
    r2 = pts[1];
    if(n1 != NULL)
        *n1 = ind[0];
    if(n2 != NULL)
        *n2 = ind[1];

    return 3;

}

// Finds the intersection of two lines, or returns false.
// The line is defined in general form at cv::Point3f(a,b,c) with 'a y + b x + c = 0'
// and the segment by p1 and p2.
bool setDegu::lineSegmentIntersection(cv::Point3f &line,
                             cv::Point2f &p1, cv::Point2f &p2, cv::Point2f &r) {

    // if b == 0 Horizontal line
    if(line.y == 0) {
        if(line.x == 0)
            return false;
        float y = -line.z/line.x;
        if(p1.y == p2.y) { //Parallel?
            if(fabs(p1.y - y) < /*EPS*/1e-8) {
                r.x = (p1.x + p2.x)/2.0;
                r.y = p1.y;
                return true;
            } else //Parallel
                return false;
        }
        //Not parallel
        //Check if intercept is in interval
        if(p1.y > p2.y) {
            if(p2.y > y || y > p1.y)
                return false;
        } else {
            if(p1.y > y || y > p2.y)
                return false;
        }
        if(p2.x - p1.x == 0) {
            r.x = p1.x;
            r.y = y;
        } else {
            float m = (p2.y - p1.y)/(p2.x - p1.x), n = p1.y - m*p1.x;
            r.x = (y - n)/m;
            r.y = y;
            return true;
        }
    } else if(line.x == 0) { //a == 0 Vertical line
        float x = -line.z/line.y;
        if(p1.x == p2.x) { //Parallel?
            if(fabs(p1.x - x) < /*EPS*/1e-8) {
                r.x = p1.x;
                r.y = (p1.y + p2.y)/2.0;
                return true;
            } else //Parallel
                return false;
        }
        //Not parallel
        //Check if intercept is in interval
        if(p1.x > p2.x) {
            if(p2.x > x || x > p1.x)
                return false;
        } else {
            if(p1.x > x || x > p2.x)
                return false;
        }
        float m = (p2.y - p1.y)/(p2.x - p1.x), n = p1.y - m*p1.x;
        r.x = x;
        r.y = m*x + n;
        return true;
    }
    //General case

    float m1 = -line.y/line.x, n1 = -line.z/line.x;

    if(p2.x - p1.x == 0) {
        r.x = p1.x;
        r.y = r.x*m1 + n1;
    } else {
        /*if(fabs(p2.y - p1.y) == 0.0) {
            r.y = p1.y;
            r.x = (r.y - n1)/m1;
            return true;
        }*/
        float m2 = (p2.y - p1.y)/(p2.x - p1.x),
              n2 = p1.y - m2*p1.x;

        if (fabs(m1 - m2) < /*EPS*/1e-8)
            return false;

        r.x = (n1 - n2)/(m2 - m1);
        r.y = m1*r.x + n1;
    }

    //Check if point is in interval
    if(p1.y != p2.y) {
        if(p1.y > p2.y) {
            if(p2.y > r.y || r.y > p1.y)
                return false;
        } else {
            if(p1.y > r.y || r.y > p2.y)
                return false;
        }
    }

    if(p1.x > p2.x) {
        if(p2.x > r.x || r.x > p1.x)
            return false;
    } else {
        if(p1.x > r.x || r.x > p2.x)
            return false;
    }

    return true;
}

//a y + b x + c = 0
bool setDegu::getGeneralLineForm(cv::Point2f p1, cv::Point2f p2,
                                                    cv::Point3f &rline) {

    // Horizontal line
    if(fabs(p1.y - p2.y) < /*EPS*/1e-8) {
        if(fabs(p1.x - p2.x) < /*EPS*/1e-8)
            return false;
        rline.x = 1.0;   //a
        rline.y = 0.0;   //b
        rline.z = -p1.y; //c
        return true;
    } else if(fabs(p1.x - p2.x) < /*EPS*/1e-8) { //b == 0 Vertical line
        rline.x = 0.0;   //a
        rline.y = 1.0;   //b
        rline.z = -p1.x; //c
        return true;
    }

    rline.x = p2.x - p1.x;
    rline.y = p1.y - p2.y;
    rline.z = p2.y*p1.x - p1.y*p2.x;
    return true;
}


bool setDegu::getGeneralLineForm(float x, float y,
                                                    float dx, float dy,
                                                    cv::Point3f &rline) {

    // Horizontal line
    if(fabs(dy) < /*EPS*/1e-8) {
        if(fabs(dx) < /*EPS*/1e-8)
            return false;
        rline.x = 1.0;   //a
        rline.y = 0.0;   //b
        rline.z = -y; //c
    } else if(dx < /*EPS*/1e-8) { //b == 0 Vertical line
        rline.x = 0.0;   //a
        rline.y = 1.0;   //b
        rline.z = -x; //c
    }

    rline.x = dx;
    rline.y = -dy;
    rline.z = dy*x - dx*y;
    return true;
}


double setDegu::distanceToSegment(int i1, int i2, std::vector<cv::Point> &contour) {
    int i, x, y, dx, dy, x1, y1, x2, y2, n = contour.size();
    double sum = 0;

    x1 = contour[i1%n].x;
    y1 = contour[i1%n].y;
    x2 = contour[i2%n].x;
    y2 = contour[i2%n].y;
    dx = x2 - x1;
    dy = y2 - y1;
    if(dx == 0) {
        for(i=i1+1; i<i2; i++)
            sum += fabs(contour[i].x - x1);
    } else if(dy == 0) {
        for(i=i1+1; i<i2; i++)
            sum += fabs(contour[i].y - y1);
    } else {
        double m = dy/(double)dx, n = y1 - m*x1, f = sqrt(m*m + 1);
        for(i=i1+1; i<i2; i++)
            sum += fabs(m*contour[i].x + n - contour[i].y)/f;
    }

    return sum;
}



bool setDegu::movementFound(cv::Mat f, int wsize, int i0, int j0) {
    int i, j, diff = wsize/2;
    int w = f.cols, h = f.rows;

    for(i=i0-diff; i<=i0+diff; i++)
        if(i>=0 && i<h)
            for(j=j0-diff; j<=j0+diff; j++)
                if(j>=0  && j < w)
                    if(f.data[i*f.step + j] != 0)
                        return true;
    return false;
}

bool setDegu::movementFound(cv::Mat f, int wsize, int i0, int j0, cv::Rect roi) {
    int i, j, diff = wsize/2,
        y1 = f.rows, x1 = f.cols;

    for(i=i0-diff; i<=i0+diff; i++)
        if(i>=0 && i<y1)
            for(j=j0-diff; j<=j0+diff; j++)
                if(j>=0  && j < x1)
                    if(f.data[i*f.step + j] != 0)
                        return true;
    return false;
}

int setDegu::movementCount(cv::Mat f, int wsize, int i0, int j0, cv::Rect roi) {
    int i, j, diff = wsize/2, count = 0,
        y1 = f.rows, x1 = f.cols;

    for(i=i0-diff; i<=i0+diff; i++)
        if(i>=0 && i<y1)
            for(j=j0-diff; j<=j0+diff; j++)
                if(j>=0  && j < x1)
                    if(f.data[i*f.step + j] != 0)
                        count++;
    return count;
}


double setDegu::histogramDistance(cv::MatND h1, cv::MatND h2) {
    cv::MatND d(h1.size(), CV_32FC1);
    cv::absdiff(h1, h2, d);
    cv::Scalar s = cv::sum(d);
    return s.val[0];
}


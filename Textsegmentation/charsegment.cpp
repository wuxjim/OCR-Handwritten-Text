#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include<fstream>
#include<iostream>
#include<stdlib.h>
using namespace std;
using namespace cv;
struct data
{
  int img[28][28];
} s1;
struct CONT
{
  int topx,topy,width,height;
}cntr;
int main(int argc,char *argv[])
{
  if(argc!=2) exit(0);
  int r,s;
  Point p,q;
  //Mat img2=imread("sample3.png",CV_8UC1);
//  Mat imgray=imread(argv[1],CV_8UC1);
  Mat img4=imread(argv[1]);
  Mat img(600,800,CV_8UC3);Mat img1(600,800,CV_8UC1);
  resize(img4,img,img.size());
  // imshow("dsas",img);
  // waitKey(0);
 cvtColor(img,img,CV_BGR2GRAY);
 // Mat img(img2);
  // Rect rec(20,20,img2.size().width/4,img2.size().height/4);
  // Mat img3=img2(rec);
  // resize(img3,img,img.size());
  GaussianBlur(img, img, Size(3,3), 1, 0, 2);
  //adaptiveThreshold(img,img, 255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 9, 2);
  threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  dilate(img, img, Mat());
  erode(img,img,Mat());
  // imshow("dsas",img);
  // waitKey(0);
  Mat mserOutMask(img.size(),CV_8UC1,Scalar(0));
  Ptr<MSER> mserExtractor  = MSER::create();

  vector<vector<Point> > mserContours;
  vector<KeyPoint> mserKeypoint;
  vector<Rect> mserBbox;
  mserExtractor->detectRegions(img, mserContours,  mserBbox);

  for (int i=0; i<mserContours.size(); ++i){
      for (int j=0; j<mserContours[i].size(); ++j){
        Point p =  mserContours[i][j];
          // cout << p.x << ", " << p.y << endl;
          mserOutMask.at<uchar>(p.y, p.x) = 255;
      }
  }
  imshow("",mserOutMask);
  Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    threshold(mserOutMask, threshold_output, 100, 255, THRESH_BINARY );
  // imshow(" ",threshold_output);
    findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ )
       { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
         boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       }
int *boundpres=(int*)calloc(contours.size(),sizeof(int));
Rect z;
Point pt;
float avght=0;
vector<Point> centers(contours.size());
for(size_t i=0;i<contours.size();i++)
{
  centers[i].x=boundRect[i].x+boundRect[i].width/2;
  centers[i].y=boundRect[i].y+boundRect[i].height/2;
  avght=(avght*i+boundRect[i].height)*1.0/(i+1);
}
//cout<<"Avght: "<<avght<<endl;
for(size_t i=0;i<contours.size();i++)
for(size_t j=i+1;j<contours.size();j++)
{
  if(centers[i].y>centers[j].y)
{
  z=boundRect[j];
  boundRect[j]=boundRect[i];
  boundRect[i]=z;
  pt=centers[j];
  centers[j]=centers[i];
  centers[i]=pt;
}
}
for(size_t i=0;i<contours.size();i++)
for(size_t j=i+1;j<contours.size();j++)
{
  if(centers[i].x>centers[j].x&&centers[i].y-centers[j].y<avght/2&&centers[i].y-centers[j].y>-1*avght/2)
{
  z=boundRect[j];
  boundRect[j]=boundRect[i];
  boundRect[i]=z;
  pt=centers[j];
  centers[j]=centers[i];
  centers[i]=pt;
}
}

// for(size_t i=0;i<contours.size();i++)
// {
//   cout<<centers[i].x<<" "<<centers[i].y<<endl;
// }
for(size_t i=0;i<contours.size();i++)
{
  if(boundRect[i].height<avght/2)
  boundpres[i]=1;
}
ofstream f("charboxes.csv");
Mat m1;
for(size_t j=0;j<contours.size();j++)
{
  if(boundpres[j]==0){
  Mat crop = img(boundRect[j]);
  if(boundRect[j].width>=boundRect[j].height)
    resize(crop,m1,Size(28,28));
    else
    {
      int wid=boundRect[j].width*28/boundRect[j].height;
      resize(crop,m1,Size(wid,28));
      Mat m2(m1);
      copyMakeBorder(m2,m1,0,0,(28-wid)/2,28-wid-(28-wid)/2,BORDER_CONSTANT,Scalar(255));
    }
//erode(m1,m1,Mat());
//erode(m1,m1,Mat());
threshold(m1, m1, 100, 255, THRESH_BINARY );
//  cout<<m1.rows<<" "<<m1.cols<<endl;
  // imshow("bbox",m1);
  // waitKey(0);

for(int x=0;x<28;x++)
{
  for(int y=0;y<28;y++)
{
  s1.img[y][x]=m1.at<uchar>(y,x);
  f<<s1.img[y][x]<<",";
}
  f<<"\n";
}
//f<<"\n";
}}
f.close();
int contrsize=0;
for( size_t i = 0; i < contours.size(); i++ )
   {
     if(boundpres[i]==0)
     {
       rectangle(img,boundRect[i],Scalar(0,0,0),1,8,0);
       contrsize++;
     }
    }

// imshow("mser", img);
// waitKey(0);

//call python file
float avgdis=0.0,dis,counter=0;
for(size_t i=0;i<contours.size();i++)
{
  if(boundpres[i]==0)
  {
    int j;
    for(j=i+1;j<contours.size();j++)
      if(boundpres[j]==0)
        break;
    if(j==contours.size())
    {
      break;
    }
    if(centers[j].y-centers[i].y>avght/2)
      {
        i=j-1;
        continue;
      }
    else
    {
      dis=boundRect[j].x-boundRect[i].width-boundRect[i].x;
      avgdis=(avgdis*counter+dis)*1.0/(counter+1);
      counter++;
      i=j-1;
    }
  }
}
//cout<<"Avgdis "<<avgdis<<"\n";
ifstream filein("prediction.txt");
ofstream fileout("output.txt");
size_t i;
for(i=0;i<contours.size()&&!filein.eof();i++)
{
  if(boundpres[i]==0)
  {
    char c;
    filein.get(c);
    // Mat m1=img(boundRect[i]);
    // imshow("wind",m1);
    // cout<<c<<endl;
    // waitKey(0);
    int j;
    for(j=i+1;j<contours.size();j++)
      if(boundpres[j]==0)
        break;
    if(j==contours.size())
    {
      fileout<<c;
      break;
    }
    if(centers[j].y-centers[i].y>avght/2)
      {
        fileout<<c;
        fileout<<"\n";
        i=j-1;
        continue;
      }
    else
    {
      if(boundRect[j].x-boundRect[i].width-boundRect[i].x>avgdis*1.5)
        {
          fileout<<c;
          fileout<<" ";
          i=j-1;
          continue;
        }
      else
      {
        fileout<<c;
        i=j-1;
        continue;
      }
    }
  }
}
//cout<<i-contours.size()<<" "<<contours.size()<<" "<<contrsize<<endl;
filein.close();
fileout.close();
  return 0;
}

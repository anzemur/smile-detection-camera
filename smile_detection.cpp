#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <ctime>
#include <string>

using namespace cv;
using namespace std;


static bool picture = false;
static bool delay = true;
static time_t triggerTime;

void timer(Mat frame, int frameHeight, int frameWidth, String text);
void detectCascades(Mat frame, CascadeClassifier faceCascade, CascadeClassifier smileCascade, double scale);
void takePicture(time_t triggerTime, Mat frame);


int main() {
	VideoCapture camera(0);
	//camera.set(CV_CAP_PROP_CONTRAST, 90);
	
	if (!camera.isOpened()) {
		cout << "Can't open the camera!" << endl;
		return -1;
	}

	namedWindow("Slika");
	

	Mat frame; // matrika za okvir slike
	


	camera.read(frame);
	//resize(frame, frame, Size(1000, 640));
	Size s = frame.size();

	int frameHeight = s.height;
	int frameWidth = s.width;

	printf("Height: %d\n", frameHeight);
	printf("Width: %d\n", frameWidth);
	

	CascadeClassifier faceCascade;
	CascadeClassifier smileCascade;
	if (!faceCascade.load("PATH TO CASCADES")) {
		cout << " Face cascace couldn't be loaded!" << endl;
		return -1;
	}
	
	if (!smileCascade.load("PATH TO CASCADES")) {
		cout << " Smile cascace couldn't be loaded!" << endl;
		return -1;
	}
	
	
	
	
	


	double scale = 1.0;
	while (true) {
		
		
		camera.retrieve(frame, CV_CAP_OPENNI_BGR_IMAGE);
		//resize(frame, frame, Size(1000, 640));

		
		detectCascades(frame, faceCascade, smileCascade, scale);
		
		
		//rectangle(frame, Point(frameWidth/4, frameHeight/4), Point(frameWidth*3/4, frameHeight*3/4), Scalar(0, 255, 0), 1, 8, 0);

		imshow("Slika", frame);


		if (picture) {
			printf("%d\n", int(time(0) - triggerTime));
			if (time(0) - triggerTime > 3 && delay) {
				timer(frame, frameHeight, frameWidth, "NANA");
				imwrite("smiling.png", frame);
				printf("PICTURE");
				delay = false;

			} else if (time(0) - triggerTime > 5) {
				picture = false;
				delay = true;
			}

		}


		int p = waitKey(30);
		if (p == 'p') {
			imwrite("out.png", frame);
		}

		if (waitKey(30) >= 0) {
			break;
		}

		

	}

	


	return 0;
}

void timer(Mat frame, int frameHeight, int frameWidth, String text) {
	putText(frame, //target image
		text, //text
		Point(frameHeight/2, frameWidth/2), //top-left position
		FONT_HERSHEY_DUPLEX,
		1.0,
		CV_RGB(255, 255, 255), //font color
		2);

}

void detectCascades(Mat frame, CascadeClassifier faceCascade, CascadeClassifier smileCascade, double scale) {
	
	Mat grayScale, sImg;
	cvtColor(frame, grayScale, COLOR_BGR2GRAY);

	double fx = 1 / scale;
	resize(grayScale, sImg, Size(), fx, fx, INTER_LINEAR_EXACT);
	equalizeHist(sImg, sImg);

	vector<Rect> faces;
	faceCascade.detectMultiScale(frame, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(140, 140));

	for (int i = 0; i < faces.size(); i++) {
		rectangle(frame, faces[i], Scalar(0, 255, 0), 1, 8, 0);

		Rect faceRect = faces[i];
		Mat smileFrame;

		//We want to concentrate only on the lower half of the detected face
		int halfFaceRect = cvRound((float)faceRect.height / 2);
		faceRect.y = faceRect.y + halfFaceRect;
		faceRect.height = halfFaceRect - 1;

		smileFrame = sImg(faceRect);

		vector<Rect> smiles;
		smileCascade.detectMultiScale(smileFrame, smiles, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(80, 80));
		for (int j = 0; j < smiles.size(); j++) {
			rectangle(frame, Point(smiles[j].x + faceRect.x, smiles[j].y + faceRect.y), Point(smiles[j].x + faceRect.x + smiles[j].width, smiles[j].y + faceRect.y + smiles[j].height), Scalar(0, 255, 0), 1, 8, 0);
			if (!picture) {
				picture = true;
				triggerTime = time(0);
			}

		}
	}


}

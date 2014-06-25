// AudioBeam.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// This source code is licensed under the MIT license. Please see the License in License.txt.
// "This is preliminary software and/or hardware and APIs are preliminary and subject to change."
//

#include "stdafx.h"
#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if( pInterfaceToRelease != NULL ){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

// Visualize Audio Beam
void drowResult( cv::Mat &img, const double angle, const double confidence )
{
	int x = img.cols / 2;
	double length = 240;
	double baseAngle = 90.0f * M_PI / 180.0f;

	img = cv::Scalar( 255, 255, 255 );

	// Drow Scale
	cv::line( img, cv::Point( x, 0 ), cv::Point( x + static_cast<int>( length * std::cos( -baseAngle ) ), static_cast<int>( length * -std::sin( -baseAngle ) ) ), cv::Scalar( 0, 0, 0 ), 1, CV_AA );
	for( int degree = 0; degree <= 50; degree += 10 ){
		cv::line( img, cv::Point( x, 0 ), cv::Point( x + static_cast<int>( length * std::cos( -( baseAngle + ( degree * M_PI / 180.0f ) ) ) ), static_cast<int>( length * -std::sin( -( baseAngle + ( degree * M_PI / 180.0f ) ) ) ) ), cv::Scalar( 0, 0, 0 ), 1, CV_AA );
		cv::line( img, cv::Point( x, 0 ), cv::Point( x + static_cast<int>( length * std::cos( -( baseAngle - ( degree * M_PI / 180.0f ) ) ) ), static_cast<int>( length * -std::sin( -( baseAngle - ( degree * M_PI / 180.0f ) ) ) ) ), cv::Scalar( 0, 0, 0 ), 1, CV_AA );
	}

	// Drow Angle of Audio Beam
	cv::line( img, cv::Point( x, 0 ), cv::Point( x + static_cast<int>( length * std::cos( angle - baseAngle ) ), static_cast<int>( length * -std::sin( angle - baseAngle ) ) ), cv::Scalar( 0, 0, 255 ), 5, CV_AA ); // Beamforming

	// Angle and Confidence
	std::ostringstream stream;
	stream << ( angle * 180.0f ) / M_PI;
	cv::putText( img, "angle : " + stream.str(), cv::Point( 20, 350 ), cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar( 0, 0, 0 ), 1, CV_AA );

	stream.str( "" );
	stream << confidence;
	cv::putText( img, "confidence : " + stream.str(), cv::Point( 20, 400 ), cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar( 0, 0, 0 ), 1, CV_AA );
}

int _tmain( int argc, _TCHAR* argv[] )
{
	cv::setUseOptimized( true );

	// Sensor
	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor( &pSensor );
	if( FAILED( hResult ) ){
		std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// Source
	IAudioSource* pAudioSource;
	hResult = pSensor->get_AudioSource( &pAudioSource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_AudioSource()" << std::endl;
		return -1;
	}

	// Get Audio Beam List
	IAudioBeamList* pAudioBeamList;
	hResult = pAudioSource->get_AudioBeams( &pAudioBeamList );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioSource::get_AudioBeams()" << std::endl;
		return -1;
	}

	// Open Audio Beam
	IAudioBeam* pAudioBeam;
	hResult = pAudioBeamList->OpenAudioBeam( 0, &pAudioBeam );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioBeamList::OpenAudioBeam()" << std::endl;
		return -1;
	}

	// Open Audio Input Stream 
	IStream* pAudioStream;
	hResult = pAudioBeam->OpenInputStream( &pAudioStream );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IAudioBeam::OpenInputStream()" << std::endl;
		return -1;
	}

	static const int samplingFrequency = 16000; // Sampling Frequency 16000[Hz]
	static const int audioBufferLength = samplingFrequency / 1000;

	cv::Mat audioMat( 480, 640, CV_8UC3 );
	cv::namedWindow( "Audio", cv::WINDOW_AUTOSIZE );

	while( 1 ){
		// Retrieved Audio Data
		FLOAT audioBuffer[audioBufferLength];
		DWORD readSize = 0;

		hResult = pAudioStream->Read( reinterpret_cast<void*>( audioBuffer ), sizeof( audioBuffer ), &readSize );

		if( readSize > 0 ){
			// Get Angle and Condidence
			FLOAT angle = 0.0f;
			FLOAT confidence = 0.0f;

			pAudioBeam->get_BeamAngle( &angle );
			pAudioBeam->get_BeamAngleConfidence( &confidence );

			if( confidence > 0 ){
				std::cout << "Angle : " << ( angle * 180.0f ) / M_PI << ", Confidence : " << confidence << std::endl;
				drowResult( audioMat, angle, confidence );
			}
		}

		cv::imshow( "Audio", audioMat );

		if( cv::waitKey( 30 ) == VK_ESCAPE ){
			break;
		}
	}

	SafeRelease( pAudioStream );
	SafeRelease( pAudioBeam );
	SafeRelease( pAudioBeamList );
	SafeRelease( pAudioSource );
	if( pSensor ){
		pSensor->Close();
	}
	SafeRelease( pSensor );

	return 0;
}


// Gesture.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// This source code is licensed under the MIT license. Please see the License in License.txt.
// "This is preliminary software and/or hardware and APIs are preliminary and subject to change."
//

#include "stdafx.h"
#include <Windows.h>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include <opencv2/opencv.hpp>


template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if( pInterfaceToRelease != NULL ){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
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
	IColorFrameSource* pColorSource;
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
		return -1;
	}

	IBodyFrameSource* pBodySource;
	hResult = pSensor->get_BodyFrameSource( &pBodySource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
		return -1;
	}

	IVisualGestureBuilderFrameSource* pGestureSource;
	hResult = CreateVisualGestureBuilderFrameSource( pSensor, 0, &pGestureSource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : CreateVisualGestureBuilderFrameSource()" << std::endl;
		return -1;
	}

	// Reader
	IColorFrameReader* pColorReader;
	hResult = pColorSource->OpenReader( &pColorReader );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	IBodyFrameReader* pBodyReader;
	hResult = pBodySource->OpenReader( &pBodyReader );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	IVisualGestureBuilderFrameReader* pGestureReader;
	hResult = pGestureSource->OpenReader( &pGestureReader );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IVisualGestureBuilderFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	// Description
	IFrameDescription* pDescription;
	hResult = pColorSource->get_FrameDescription( &pDescription );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
		return -1;
	}

	int width = 0;
	int height = 0;
	pDescription->get_Width( &width ); // 1920
	pDescription->get_Height( &height ); // 1080
	unsigned int bufferSize = width * height * 4 * sizeof( unsigned char );

	cv::Mat bufferMat( height, width, CV_8UC4 );
	cv::Mat bodyMat( height / 2, width / 2, CV_8UC4 );
	cv::namedWindow( "Body" );

	// Color Table
	cv::Vec3b color[6];
	color[0] = cv::Vec3b( 255, 0, 0 );
	color[1] = cv::Vec3b( 0, 255, 0 );
	color[2] = cv::Vec3b( 0, 0, 255 );
	color[3] = cv::Vec3b( 255, 255, 0 );
	color[4] = cv::Vec3b( 255, 0, 255 );
	color[5] = cv::Vec3b( 0, 255, 255 );

	// Coordinate Mapper
	ICoordinateMapper* pCoordinateMapper;
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
		return -1;
	}

	// Create Gesture Dataase from File (*.gba)
	IVisualGestureBuilderDatabase* pGestureDatabase;
	hResult = CreateVisualGestureBuilderDatabaseInstanceFromFile( L"HandUp.gba", &pGestureDatabase );
	if( FAILED( hResult ) ){
		std::cerr << "Error : CreateVisualGestureBuilderDatabaseInstanceFromFile()" << std::endl;
		return -1;
	}

	// Add Gesture
	UINT gestureCount = 0;
	hResult = pGestureDatabase->get_AvailableGesturesCount( &gestureCount );
	if( FAILED( hResult ) && ( gestureCount != 0 ) ){
		std::cerr << "Error : IVisualGestureBuilderDatabase::get_AvailableGesturesCount()" << std::endl;
		return -1;
	}

	IGesture* pGesture;
	hResult = pGestureDatabase->get_AvailableGestures( 0, &pGesture );
	if( SUCCEEDED( hResult ) ){
		hResult = pGestureSource->AddGesture( pGesture );
		hResult = pGestureSource->SetIsEnabled( pGesture, true );
		if( FAILED( hResult ) ){
			std::cerr << "Error : IVisualGestureBuilderFrameSource::AddGesture()" << std::endl;
			return -1;
		}
	}

	while( 1 ){
		// Frame
		IColorFrame* pColorFrame = nullptr;
		hResult = pColorReader->AcquireLatestFrame( &pColorFrame );
		if( SUCCEEDED( hResult ) ){
			hResult = pColorFrame->CopyConvertedFrameDataToArray( bufferSize, reinterpret_cast<BYTE*>( bufferMat.data ), ColorImageFormat_Bgra );
			if( SUCCEEDED( hResult ) ){
				cv::resize( bufferMat, bodyMat, cv::Size(), 0.5, 0.5 );
			}
		}
		SafeRelease( pColorFrame );

		UINT64 trackingId = _UI64_MAX;
		IBodyFrame* pBodyFrame = nullptr;
		hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
		if( SUCCEEDED( hResult ) ){
			IBody* pBody[BODY_COUNT] = { 0 };
			hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );
			if( SUCCEEDED( hResult ) ){
				for( int count = 0; count < BODY_COUNT; count++ ){
					BOOLEAN bTracked = false;
					hResult = pBody[count]->get_IsTracked( &bTracked );
					if( SUCCEEDED( hResult ) && bTracked ){
						Joint joint[JointType::JointType_Count];
						hResult = pBody[count]->GetJoints( JointType::JointType_Count, joint );
						if( SUCCEEDED( hResult ) ){
							// Joint
							for( int type = 0; type < JointType::JointType_Count; type++ ){
								ColorSpacePoint colorSpacePoint = { 0 };
								pCoordinateMapper->MapCameraPointToColorSpace( joint[type].Position, &colorSpacePoint );
								int x = static_cast<int>( colorSpacePoint.X );
								int y = static_cast<int>( colorSpacePoint.Y );
								if( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ){
									cv::circle( bufferMat, cv::Point( x, y ), 5, static_cast< cv::Scalar >( color[count] ), -1, CV_AA );
								}
							}
						}

						// Set TrackingID to Detect Gesture 
						pBody[count]->get_TrackingId( &trackingId );
						pGestureSource->put_TrackingId( trackingId );
					}
				}
				cv::resize( bufferMat, bodyMat, cv::Size(), 0.5, 0.5 );
			}
		}
		SafeRelease( pBodyFrame );

		// Detect Gesture
		IVisualGestureBuilderFrame* pGestureFrame = nullptr;
		hResult = pGestureReader->CalculateAndAcquireLatestFrame( &pGestureFrame );
		if( SUCCEEDED( hResult ) ){
			IDiscreteGestureResult* pGestureResult = nullptr;
			hResult = pGestureFrame->get_DiscreteGestureResult( pGesture, &pGestureResult );
			if( SUCCEEDED( hResult ) ){
				BOOLEAN bDetected = false;
				float confidence = 0.0f;
				hResult = pGestureResult->get_Detected( &bDetected );
				if( SUCCEEDED( hResult ) && bDetected ){
					std::cout << "Detected Gesture" << std::endl;
				}
			}
			SafeRelease( pGestureResult );
		}
		SafeRelease( pGestureFrame );

		cv::imshow( "Body", bodyMat );

		if( cv::waitKey( 10 ) == VK_ESCAPE ){
			break;
		}
	}

	SafeRelease( pColorSource );
	SafeRelease( pBodySource );
	SafeRelease( pGestureSource );
	SafeRelease( pColorReader );
	SafeRelease( pBodyReader );
	SafeRelease( pGestureReader );
	SafeRelease( pDescription );
	SafeRelease( pCoordinateMapper );
	SafeRelease( pGestureDatabase );
	if( pSensor ){
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();

	return 0;
}


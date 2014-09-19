// HDFace.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// This source code is licensed under the MIT license. Please see the License in License.txt.
// "This is preliminary software and/or hardware and APIs are preliminary and subject to change."
//

#include "stdafx.h"
#include <Windows.h>
#include <Kinect.h>
#include <Kinect.Face.h>
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
	cv::Mat faceMat( height / 2, width / 2, CV_8UC4 );
	cv::namedWindow( "HDFace" );

	// Color Table
	cv::Vec3b color[BODY_COUNT];
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

	IHighDefinitionFaceFrameSource* pHDFaceSource[BODY_COUNT];
	IHighDefinitionFaceFrameReader* pHDFaceReader[BODY_COUNT];
	IFaceAlignment* pFaceAlignment[BODY_COUNT];
	IFaceModel* pFaceModel[BODY_COUNT];
	std::vector<std::vector<float>> deformations( BODY_COUNT, std::vector<float>( FaceShapeDeformations::FaceShapeDeformations_Count ) );
	for( int count = 0; count < BODY_COUNT; count++ ){
		// Source
		hResult = CreateHighDefinitionFaceFrameSource( pSensor, &pHDFaceSource[count] );
		if( FAILED( hResult ) ){
			std::cerr << "Error : CreateHighDefinitionFaceFrameSource()" << std::endl;
			return -1;
		}

		// Reader
		hResult = pHDFaceSource[count]->OpenReader( &pHDFaceReader[count] );
		if( FAILED( hResult ) ){
			std::cerr << "Error : IHighDefinitionFaceFrameSource::OpenReader()" << std::endl;
			return -1;
		}

		hResult = pHDFaceSource[count]->OpenReader( &pHDFaceReader[count] );
		if( FAILED( hResult ) ){
			std::cerr << "Error : IHighDefinitionFaceFrameSource::OpenReader()" << std::endl;
			return -1;
		}

		// Create Face Alignment
		hResult = CreateFaceAlignment( &pFaceAlignment[count] );
		if( FAILED( hResult ) ){
			std::cerr << "Error : CreateFaceAlignment()" << std::endl;
			return -1;
		}

		// Create Face Model
		hResult = CreateFaceModel( 1.0f, FaceShapeDeformations::FaceShapeDeformations_Count, &deformations[count][0], &pFaceModel[count] );
		if( FAILED( hResult ) ){
			std::cerr << "Error : CreateFaceModel()" << std::endl;
			return -1;
		}
	}

	UINT32 vertex = 0;
	hResult = GetFaceModelVertexCount( &vertex ); // 1347
	if( FAILED( hResult ) ){
		std::cerr << "Error : GetFaceModelVertexCount()" << std::endl;
		return -1;
	}

	while( 1 ){
		// Color Frame
		IColorFrame* pColorFrame = nullptr;
		hResult = pColorReader->AcquireLatestFrame( &pColorFrame );
		if( SUCCEEDED( hResult ) ){
			hResult = pColorFrame->CopyConvertedFrameDataToArray( bufferSize, reinterpret_cast<BYTE*>( bufferMat.data ), ColorImageFormat_Bgra );
			if( SUCCEEDED( hResult ) ){
				cv::resize( bufferMat, faceMat, cv::Size(), 0.5, 0.5 );
			}
		}
		SafeRelease( pColorFrame );

		// Body Frame
		IBodyFrame* pBodyFrame = nullptr;
		hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
		if( SUCCEEDED( hResult ) ){
			IBody* pBody[BODY_COUNT] = { 0 };
			hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );
			if( SUCCEEDED( hResult ) ){
				for( int count = 0; count < BODY_COUNT; count++ ){
					BOOLEAN bTrackingIdValid = false;
					hResult = pHDFaceSource[count]->get_IsTrackingIdValid( &bTrackingIdValid );
					if( !bTrackingIdValid ){
						BOOLEAN bTracked = false;
						hResult = pBody[count]->get_IsTracked( &bTracked );
						if( SUCCEEDED( hResult ) && bTracked ){
							//// Joint
							//Joint joint[JointType::JointType_Count];
							//hResult = pBody[count]->GetJoints( JointType::JointType_Count, joint );
							//if( SUCCEEDED( hResult ) ){
							//	for( int type = 0; type < JointType::JointType_Count; type++ ){
							//		ColorSpacePoint colorSpacePoint = { 0 };
							//		pCoordinateMapper->MapCameraPointToColorSpace( joint[type].Position, &colorSpacePoint );
							//		int x = static_cast< int >( colorSpacePoint.X );
							//		int y = static_cast< int >( colorSpacePoint.Y );
							//		if( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ){
							//			cv::circle( bufferMat, cv::Point( x, y ), 5, static_cast< cv::Scalar >( color[count] ), -1, CV_AA );
							//		}
							//	}
							//}

							// Set TrackingID to Detect Face
							UINT64 trackingId = _UI64_MAX;
							hResult = pBody[count]->get_TrackingId( &trackingId );
							if( SUCCEEDED( hResult ) ){
								pHDFaceSource[count]->put_TrackingId( trackingId );
							}
						}
					}
				}
			}
		}
		SafeRelease( pBodyFrame );

		// HD Face Frame
		for( int count = 0; count < BODY_COUNT; count++ ){
			IHighDefinitionFaceFrame* pHDFaceFrame = nullptr;
			hResult = pHDFaceReader[count]->AcquireLatestFrame( &pHDFaceFrame );
			if( SUCCEEDED( hResult ) && pHDFaceFrame != nullptr ){
				BOOLEAN bFaceTracked = false;
				hResult = pHDFaceFrame->get_IsFaceTracked( &bFaceTracked );
				if( SUCCEEDED( hResult ) && bFaceTracked ){
					hResult = pHDFaceFrame->GetAndRefreshFaceAlignmentResult( pFaceAlignment[count] );
					if( SUCCEEDED( hResult ) && pFaceAlignment[count] != nullptr ){
						std::vector<CameraSpacePoint> facePoints( 1347 );
						hResult = pFaceModel[count]->CalculateVerticesForAlignment( pFaceAlignment[count], vertex, &facePoints[0] );
						if( SUCCEEDED( hResult ) ){
							for( int point = 0; point < vertex; point++ ){
								ColorSpacePoint colorSpacePoint;
								hResult = pCoordinateMapper->MapCameraPointToColorSpace( facePoints[point], &colorSpacePoint );
								if( SUCCEEDED( hResult ) ){
									int x = static_cast<int>( colorSpacePoint.X );
									int y = static_cast<int>( colorSpacePoint.Y );
									if( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ){
										cv::circle( bufferMat, cv::Point( static_cast< int >( colorSpacePoint.X ), static_cast< int >( colorSpacePoint.Y ) ), 5, static_cast< cv::Scalar >( color[count] ), -1, CV_AA );
									}
								}
							}
						}
					}
				}
			}
			SafeRelease( pHDFaceFrame );
		}

		cv::resize( bufferMat, faceMat, cv::Size(), 0.5, 0.5 );
		cv::imshow( "HDFace", faceMat );

		if( cv::waitKey( 10 ) == VK_ESCAPE ){
			break;
		}
	}

	SafeRelease( pColorSource );
	SafeRelease( pBodySource );
	SafeRelease( pColorReader );
	SafeRelease( pBodyReader );
	SafeRelease( pCoordinateMapper );
	for( int count = 0; count < BODY_COUNT; count++ ){
		SafeRelease( pHDFaceSource[count] );
		SafeRelease( pHDFaceReader[count] );
		SafeRelease( pFaceAlignment[count] );
		SafeRelease( pFaceModel[count] );
	}
	if( pSensor ){
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();

	return 0;
}


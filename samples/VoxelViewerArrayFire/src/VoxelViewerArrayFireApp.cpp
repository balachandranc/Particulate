#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <particulate/Rle.hpp>

#include <arrayfire.h>
#include <af/util.h>

typedef af::array		Array;
typedef unsigned long	Index;
typedef float			Scalar;

static const Index kAxisLen = 30;
static const Index kDataLen = kAxisLen * kAxisLen * kAxisLen;

using namespace ci;
using namespace ci::app;
using namespace std;

static ci::gl::VertBatchRef loadVoxels(const ci::fs::path& filePath)
{
	Array voxel_data = af::readArray( filePath.c_str(), "voxels" );
	float half_ext   = kAxisLen / 2.0;
	
	ci::vec3 pos;
	
	ci::gl::VertBatchRef batch = ci::gl::VertBatch::create( GL_POINTS );
	
	for(size_t idx = 0; idx < kDataLen; idx++)
	{
		if( ( voxel_data( idx ).scalar<Scalar>() > 1e-3 ) )
		{
			pos.x =   ( idx % kAxisLen )              - half_ext;
			pos.y = ( ( idx / kAxisLen ) % kAxisLen ) - half_ext;
			pos.z = ( ( idx / kAxisLen ) / kAxisLen ) - half_ext;
			
			batch->color( ci::ColorA( 1.0, 1.0, 1.0, 1.0 ) );
			batch->vertex( pos );
		}
	}
	
	return batch;
}

Array convertSample(const ci::fs::path& filePath)
{
	Array groupMat = Array( kDataLen );
	
	{
		Scalar* data_temp  = new Scalar[ kDataLen ];
		
		std::ifstream fh( filePath.c_str() );
		if( fh.is_open() )
		{
			{
				particulate::rle::IStream rleStream( fh );
				
				Index data_len = rleStream.read<Scalar>( &data_temp, false );
				assert( data_len == kDataLen );
				
				groupMat = Array( data_len, data_temp );
			}
			fh.close();
		}
		fh.clear();
		
		delete[] data_temp;
	}
	
	return groupMat;
}

class VoxelViewerArrayFireApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;
	
	ci::gl::VertBatchRef	mBatch;
	ci::CameraPersp			mCam;
};

void VoxelViewerArrayFireApp::setup()
{
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1, 1e5 );
	
	
	std::string  modelName    = "stairs";
	
	ci::fs::path modelRlePath = getAssetPath( modelName + ".rle" );
	ci::fs::path modelAfPath  = modelRlePath.parent_path() / ( modelName + ".array" );
	
	/*
	// Convert model from RLE to ArrayFire and save:
	Array converted = convertSample( modelRlePath );
	af::saveArray( "voxels", converted, modelAfPath.c_str(), false );
	*/
	
	mBatch = loadVoxels( modelAfPath );
}

void VoxelViewerArrayFireApp::mouseDown( MouseEvent event )
{
}

void VoxelViewerArrayFireApp::update()
{
}

void VoxelViewerArrayFireApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	ci::vec3 camPos = ci::vec3( sin( getElapsedSeconds() ), cos( getElapsedSeconds() ), cos( getElapsedSeconds() ) ) * 60.0f;
	ci::vec3 camTar = ci::vec3( 0.0 );
	
	mCam.lookAt( camPos, camTar );
	gl::setMatrices( mCam );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	if( mBatch )
		mBatch->draw();
}

void VoxelViewerArrayFireApp::resize()
{
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1, 1e6 );
}

CINDER_APP( VoxelViewerArrayFireApp, RendererGl )

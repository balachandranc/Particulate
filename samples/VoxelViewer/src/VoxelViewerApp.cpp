#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <particulate/Rle.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

static ci::gl::VertBatchRef loadVoxels(const ci::fs::path& filePath)
{
	std::ifstream fh( filePath.c_str() );
	
	if( ! fh.is_open() )
		return nullptr;
	
	particulate::rle::IStream rleStream( fh );
	
	float* data_temp = NULL;
	size_t data_len  = rleStream.read<float>( &data_temp );
	size_t axis_len  = cbrt( data_len );
	float  half_ext  = axis_len / 2.0;
	
	fh.close();
	
	ci::vec3 pos;
	
	ci::gl::VertBatchRef batch = ci::gl::VertBatch::create( GL_POINTS );
	
	for(size_t idx = 0; idx < data_len; idx++)
	{
		if( data_temp[ idx ] )
		{
			pos.x =   ( idx % axis_len )              - half_ext;
			pos.y = ( ( idx / axis_len ) % axis_len ) - half_ext;
			pos.z = ( ( idx / axis_len ) / axis_len ) - half_ext;
			
			batch->color( ci::ColorA( 1.0, 1.0, 1.0, 1.0 ) );
			batch->vertex( pos );
		}
	}
	
	delete[] data_temp;
	
	return batch;
}

class VoxelViewerApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;
	
	ci::gl::VertBatchRef	mBatch;
	ci::CameraPersp			mCam;
};

void VoxelViewerApp::setup()
{
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1, 1e5 );
	
	mBatch = loadVoxels( getAssetPath( "stairs.rle" ) );
}

void VoxelViewerApp::mouseDown( MouseEvent event )
{
}

void VoxelViewerApp::update()
{
}

void VoxelViewerApp::draw()
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

void VoxelViewerApp::resize()
{
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1, 1e6 );
}

CINDER_APP( VoxelViewerApp, RendererGl )

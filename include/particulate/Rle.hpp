/*
 Copyright (c) 2015, Patrick J. Hebron
 All rights reserved.
 
 http://patrickhebron.com
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace particulate { namespace rle {
	
	template <typename Tp>
	static inline std::ostream& write_to_stream(std::ostream& fh, const Tp& v)
	{
		return fh.write( reinterpret_cast<const char*>( &v ), sizeof( v ) );
	}
	
	template <typename Tp>
	static inline std::istream& read_from_stream(std::istream& fh, Tp& v)
	{
		return fh.read( reinterpret_cast<char*>( &v ), sizeof( v ) );
	}
	
	struct OStream
	{
	  private:
		
		std::ostream&	mFh;
		size_t			mLen;
		size_t			mCount;
		size_t			mValue;
		
	  public:
		
		OStream(std::ostream& f) :
		mFh( f ),
		mLen( 0 ),
		mCount( 0 ),
		mValue( (std::numeric_limits<size_t>::max())() )
		{
			mFh.seekp( 0, std::ios::beg );
			write_to_stream( mFh, mLen );
		}
		
		~OStream()
		{
			if( mCount > 0 )
			{
				write_to_stream( mFh, mCount );
				write_to_stream( mFh, mValue );
			}
			
			mFh.seekp( 0, std::ios::beg );
			write_to_stream( mFh, mLen );
			mFh.seekp( 0, std::ios::end );
		}
		
		void write(size_t datum)
		{
			if( datum == mValue )
			{
				++mCount;
			}
			else
			{
				if( mCount > 0 )
				{
					write_to_stream( mFh, mCount );
					write_to_stream( mFh, mValue );
				}
				
				mValue = datum;
				mCount = 1;
			}
			
			++mLen;
		}
		
		void write(const size_t* data, size_t len)
		{
			for(size_t i = 0; i < len; i++)
			{
				write( data[ i ] );
			}
		}
	};
	
	struct IStream
	{
	  private:
		
		std::istream&	mFh;
		
	  public:
		
		IStream(std::istream& f) :
			mFh( f )
		{
			/* no-op */
		}
		
		~IStream()
		{
			/* no-op */
		}
		
		template <typename Tp>
		size_t read(Tp** data, bool init = true)
		{
			size_t len, value, count, iter = 0;
			
			read_from_stream( mFh, len );
			
			if( init )
				*data = new Tp[ len ];
			
			while( read_from_stream( mFh, count ) )
			{
				read_from_stream( mFh, value );
				
				for(size_t i = 0; i < count; i++)
				{
					( *data )[ iter ] = static_cast<Tp>( value );
					++iter;
				}
			}
			
			assert( iter == len );
			
			return len;
		}
	};
	
} } // namespace particulate::rle

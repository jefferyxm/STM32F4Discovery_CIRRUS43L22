
/* HTAB = 4 */
/****************************************************************************
 * madlld.c -- A simple program decoding an MPEG audio stream to 16-bit		*
 * PCM from stdin to stdout. This program is just a simple sample			*
 * demonstrating how the low-level libmad API can be used.					*
 *--------------------------------------------------------------------------*
 * (c) 2001--2004 Bertrand Petit											*
 *																			*
 * Redistribution and use in source and binary forms, with or without		*
 * modification, are permitted provided that the following conditions		*
 * are met:																	*
 *																			*
 * 1. Redistributions of source code must retain the above copyright		*
 *    notice, this list of conditions and the following disclaimer.			*
 *																			*
 * 2. Redistributions in binary form must reproduce the above				*
 *    copyright notice, this list of conditions and the following			*
 *    disclaimer in the documentation and/or other materials provided		*
 *    with the distribution.												*
 * 																			*
 * 3. Neither the name of the author nor the names of its contributors		*
 *    may be used to endorse or promote products derived from this			*
 *    software without specific prior written permission.					*
 * 																			*
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''		*
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED		*
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A			*
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR		*
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,				*
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT			*
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF			*
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND		*
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,		*
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT		*
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF		*
 * SUCH DAMAGE.																*
 *																			*
 ****************************************************************************/

/*
 * $Name: v1_1p1 $
 * $Date: 2004/03/19 07:13:13 $
 * $Revision: 1.20 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>   /* for pow() and log10() */
#include "mad.h"
#include "libmad_config.h"
#include "ff.h"
#include "stm32f4xx.h"
#include "board_init.h"
#include "player.h"
   

/* Should we use getopt() for command-line arguments parsing? */
#if (defined(unix) || defined (__unix__) || defined(__unix) || \
	 defined(HAVE_GETOPT)) \
	&& !defined(WITHOUT_GETOPT)
#include <unistd.h>
#define HAVE_GETOPT
#else
#include <ctype.h>
#undef HAVE_GETOPT
#endif

/****************************************************************************
 * Global variables.														*
 ****************************************************************************/


/* This table represents the subband-domain filter characteristics. It
 * is initialized by the ParseArgs() function and is used as
 * coefficients against each subband samples when DoFilter is non-nul.
 */
mad_fixed_t	Filter[32];

int			DoFilter=0;

/****************************************************************************
 * Converts a sample from libmad's fixed point number format to a signed	*
 * short (16 bits).															*
 ****************************************************************************/
static signed short MadFixedToSshort(mad_fixed_t sample)
{
    /* A fixed point number is formed of the following bit pattern:
     *
     * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
     * MSB                          LSB
     * S ==> Sign (0 is positive, 1 is negative)
     * W ==> Whole part bits
     * F ==> Fractional part bits
     *
     * This pattern contains MAD_F_FRACBITS fractional bits, one
     * should alway use this macro when working on the bits of a fixed
     * point number. It is not guaranteed to be constant over the
     * different platforms supported by libmad.
     *
     * The signed short value is formed, after clipping, by the least
     * significant whole part bit, followed by the 15 most significant
     * fractional part bits. Warning: this is a quick and dirty way to
     * compute the 16-bit number, madplay includes much better
     * algorithms.
     */

    /* Clipping */
  
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
      sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
      sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}


/****************************************************************************
 * Applies a frequency-domain filter to audio data in the subband-domain.	*
 ****************************************************************************/
static void ApplyFilter(struct mad_frame *Frame)
{
	
}

/****************************************************************************
 * Main decoding loop. This is where mad is used.							*
 ****************************************************************************/
static FATFS fatfs;
static FRESULT fresult;

size_t		ReadSize,Remaining,tmp;
unsigned char	*ReadStart;
UINT            readsize,writesize;

struct mad_stream	Stream;
struct mad_frame	Frame;
struct mad_synth	Synth;

int             readyBuffIndex = 0;
int             nowUsingBuff0 = 1;
int             nowUsingBuff1 = 0;
int             startPlay = 0;

#define INPUT_BUFFER_SIZE	(1024*8)//(1024U)//(8192)  
#define OUTPUT_BUFFER_SIZE	(2048*2)//(5120U)//8192U

unsigned char		        InputBuffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD];
unsigned short                   OutputBuffer[2][OUTPUT_BUFFER_SIZE];
unsigned short                  *OutputPtr=&OutputBuffer[0][0];
unsigned char                   *GuardPtr=NULL;
unsigned short 	                *OutputBufferEnd=&OutputBuffer[0][0]+OUTPUT_BUFFER_SIZE;
mad_timer_t			Timer;

int MpegAudioDecoder(FIL *InputFp)
{
        
        int				Status=0,i;
        unsigned long		        FrameCount=0;
        
	/* First the structures used by libmad must be initialized. */
	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
	mad_timer_reset(&Timer);

        I2S3_TX_DMAInit(&OutputBuffer[0][0], &OutputBuffer[1][0], OUTPUT_BUFFER_SIZE);
        
	/* Decoding options can here be set in the options field of the
	 * Stream structure.
	 */

	/* {1} When decoding from a file we need to know when the end of
	 * the file is reached at the same time as the last bytes are read
	 * (see also the comment marked {3} bellow). Neither the standard
	 * C fread() function nor the POSIX read() system call provides
	 * this feature. We thus need to perform our reads through an
	 * interface having this feature, this is implemented here by the
	 * bstdfile.c module.
	 */
        
	if(InputFp==NULL)
	{
            return(1);
	}

	/* This is the decoding loop. */
	do
	{
		/* The input bucket must be filled if it becomes empty or if
		 * it's the first execution of the loop.
		 */
		if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN)
		{
			/* {2} libmad may not consume all bytes of the input
			 * buffer. If the last frame in the buffer is not wholly
			 * contained by it, then that frame's start is pointed by
			 * the next_frame member of the Stream structure. This
			 * common situation occurs when mad_frame_decode() fails,
			 * sets the stream error code to MAD_ERROR_BUFLEN, and
			 * sets the next_frame pointer to a non NULL value. (See
			 * also the comment marked {4} bellow.)
			 *
			 * When this occurs, the remaining unused bytes must be
			 * put back at the beginning of the buffer and taken in
			 * account before refilling the buffer. This means that
			 * the input buffer must be large enough to hold a whole
			 * frame at the highest observable bit-rate (currently 448
			 * kb/s). XXX=XXX Is 2016 bytes the size of the largest
			 * frame? (448000*(1152/32000))/8
			 */
			if(Stream.next_frame!=NULL)
			{
				Remaining=Stream.bufend-Stream.next_frame;
				memmove(InputBuffer,Stream.next_frame,Remaining);
				ReadStart=InputBuffer+Remaining;
				ReadSize=INPUT_BUFFER_SIZE-Remaining;
			}
			else
                        {
                              ReadSize=INPUT_BUFFER_SIZE,
                              ReadStart=InputBuffer,
                              Remaining=0;
                        }
                                        

			/* Fill-in the buffer. If an error occurs print a message
			 * and leave the decoding loop. If the end of stream is
			 * reached we also leave the loop but the return status is
			 * left untouched.
			 */
                        
                        fresult = f_read(InputFp, ReadStart, ReadSize, &readsize);
			if(fresult!=FR_OK)
			{
                              Status=1;
                              break;
			}

			/* {3} When decoding the last frame of a file, it must be
			 * followed by MAD_BUFFER_GUARD zero bytes if one wants to
			 * decode that last frame. When the end of file is
			 * detected we append that quantity of bytes at the end of
			 * the available data. Note that the buffer can't overflow
			 * as the guard size was allocated but not used the the
			 * buffer management code. (See also the comment marked
			 * {1}.)
			 *
			 * In a message to the mad-dev mailing list on May 29th,
			 * 2001, Rob Leslie explains the guard zone as follows:
			 *
			 *    "The reason for MAD_BUFFER_GUARD has to do with the
			 *    way decoding is performed. In Layer III, Huffman
			 *    decoding may inadvertently read a few bytes beyond
			 *    the end of the buffer in the case of certain invalid
			 *    input. This is not detected until after the fact. To
			 *    prevent this from causing problems, and also to
			 *    ensure the next frame's main_data_begin pointer is
			 *    always accessible, MAD requires MAD_BUFFER_GUARD
			 *    (currently 8) bytes to be present in the buffer past
			 *    the end of the current frame in order to decode the
			 *    frame."
			 */
			if(f_eof(InputFp))
			{
				GuardPtr=ReadStart+ReadSize;
				memset(GuardPtr,0,MAD_BUFFER_GUARD);
				ReadSize+=MAD_BUFFER_GUARD;
                                break;
			}

			/* Pipe the new buffer content to libmad's stream decoder
                         * facility.
			 */
			mad_stream_buffer(&Stream,InputBuffer,ReadSize+Remaining);
			Stream.error=0;
		}

		/* Decode the next MPEG frame. The streams is read from the
		 * buffer, its constituents are break down and stored the the
		 * Frame structure, ready for examination/alteration or PCM
		 * synthesis. Decoding options are carried in the Frame
		 * structure from the Stream structure.
		 *
		 * Error handling: mad_frame_decode() returns a non zero value
		 * when an error occurs. The error condition can be checked in
		 * the error member of the Stream structure. A mad error is
		 * recoverable or fatal, the error status is checked with the
		 * MAD_RECOVERABLE macro.
		 *
		 * {4} When a fatal error is encountered all decoding
		 * activities shall be stopped, except when a MAD_ERROR_BUFLEN
		 * is signaled. This condition means that the
		 * mad_frame_decode() function needs more input to complete
		 * its work. One shall refill the buffer and repeat the
		 * mad_frame_decode() call. Some bytes may be left unused at
		 * the end of the buffer if those bytes forms an incomplete
		 * frame. Before refilling, the remaining bytes must be moved
		 * to the beginning of the buffer and used for input for the
		 * next mad_frame_decode() invocation. (See the comments
		 * marked {2} earlier for more details.)
		 *
		 * Recoverable errors are caused by malformed bit-streams, in
		 * this case one can call again mad_frame_decode() in order to
		 * skip the faulty part and re-sync to the next frame.
		 */
		if(mad_frame_decode(&Frame,&Stream))
		{
			if(MAD_RECOVERABLE(Stream.error))
			{
				/* Do not print a message if the error is a loss of
				 * synchronization and this loss is due to the end of
				 * stream guard bytes. (See the comments marked {3}
				 * supra for more informations about guard bytes.)
				 */
				if(Stream.error!=MAD_ERROR_LOSTSYNC ||Stream.this_frame!=GuardPtr)
				{

				}
				continue;
			}
			else if(Stream.error==MAD_ERROR_BUFLEN)
                        {
                            continue;
                        }
                        else
                        {
                                Status=1;
                                break;
                        }	
		}

		/* Accounting. The computed frame duration is in the frame
		 * header structure. It is expressed as a fixed point number
		 * whole data type is mad_timer_t. It is different from the
		 * samples fixed point format and unlike it, it can't directly
		 * be added or subtracted. The timer module provides several
		 * functions to operate on such numbers. Be careful there, as
		 * some functions of libmad's timer module receive some of
		 * their mad_timer_t arguments by value!
		 */
		FrameCount++;
		mad_timer_add(&Timer,Frame.header.duration);

		/* Between the frame decoding and samples synthesis we can
		 * perform some operations on the audio data. We do this only
		 * if some processing was required. Detailed explanations are
		 * given in the ApplyFilter() function.
		 */
		//if(DoFilter)
                    //ApplyFilter(&Frame);

		/* Once decoded the frame is synthesized to PCM samples. No errors
		 * are reported by mad_synth_frame();
		 */
		mad_synth_frame(&Synth,&Frame);

		/* Synthesized samples must be converted from libmad's fixed
		 * point number to the consumer format. Here we use unsigned
		 * 16 bit big endian integers on two channels. Integer samples
		 * are temporarily stored in a buffer that is flushed when
		 * full.
		 */
		for(i=0;i<Synth.pcm.length;i++)
		{
			signed short	Sample;

			/* Left channel */
			Sample=MadFixedToSshort(Synth.pcm.samples[0][i]);
			//*(OutputPtr++)=Sample>>8;
			//*(OutputPtr++)=Sample&0xff;
                        *(OutputPtr++)=Sample;

			/* Right channel. If the decoded stream is monophonic then
			 * the right output channel is the same as the left one.
			 */
			if(MAD_NCHANNELS(&Frame.header)==2)
                        {
                            Sample=MadFixedToSshort(Synth.pcm.samples[1][i]);
                        }	
			//*(OutputPtr++)=Sample>>8;
			//*(OutputPtr++)=Sample&0xff;
                        *(OutputPtr++)=Sample;
                        
			/* Flush the output buffer if it is full. */
			if(OutputPtr==OutputBufferEnd)
			{
                            
                            if(startPlay==0)
                            {
                                startDMA();  //开始播放
                                startPlay = 1;
                            }
                          
                            //等待前一个缓冲区数据播放完成
                            if(readyBuffIndex==0)
                            {
                                while(nowUsingBuff1==1);  //buff1 正在使用，需要等待
                            }
                            else if(readyBuffIndex==1)
                            {
                                while(nowUsingBuff0==1);  //buff0 正在使用，需要等待
                            }
  
                            if (++readyBuffIndex>=2)  //切换接收缓冲区
                            {
                                readyBuffIndex=0;
                            }

                            OutputPtr=&OutputBuffer[readyBuffIndex][0];
                            OutputBufferEnd=&OutputBuffer[readyBuffIndex][0]+OUTPUT_BUFFER_SIZE;
			}
		}
	}while(1);

	mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);
  
        stopDMA();
        startPlay = 0;
	/* That's the end of the world (in the H. G. Wells way). */
	return(Status);
}


/****************************************************************************
 * End of file madlld.c														*
 ****************************************************************************/









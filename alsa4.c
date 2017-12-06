/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <time.h>

int main() {
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;
	int voice_detected = 0;
	int sound_duration = 0;
	int second_happen = 0;
	int second_log[1000];
	int second_loop = 0;
  time_t now;
  struct tm *tm;

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params,
                                      &frames, &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                         &val, &dir);
  loops = 5000000 / val;

  //while (loops > 0) {
  while (1) {
    loops--;
		now = time(0);
		if ((tm = localtime (&now)) == NULL) {
        printf ("Error extracting time stuff\n");
        return 1;
    }
    rc = snd_pcm_readi(handle, buffer, frames);
		int i;
		int nArray = (sizeof buffer)/sizeof(buffer[0]);
		int div = 1;
		for(i = 0; i<=nArray;i++){
				int d = 0;
			if(buffer[i]>0){
				//printf("%d#",buffer[i]);
				int amp = (buffer[i])/div;
    		//for(d=0;d<buffer[i];d++)
    		for(d=0;d<amp;d++)
    		{   
      		printf("X");
    		}   
				if((buffer[i]*buffer[i])>100)
				{
					voice_detected = 1;	
					second_log[second_loop] = tm->tm_sec;
					second_loop++;
					//if(second_happen == 0){
					//	second_happen = tm->tm_sec;
					//}
				}
				else if((buffer[i]*buffer[i])<100)
				{
					voice_detected = 0;
				}
    		printf("|%d\n",buffer[i]);
			}
			else if(buffer[i]<0){
				//printf("%d#",buffer[i]);
				int amp = (buffer[i])/div;
    		//for(d=0;d<buffer[i];d++)
    		for(d=0;d>amp;d--)
    		{   
      		printf("_");
    		}   
    		printf("|-%d\n",buffer[i]);
			}
    	//printf("\n");
			for(d=0;d<(second_loop-1);d++)
			{
				sound_duration = (second_log[second_loop] - second_log[second_loop-1]) + sound_duration;
			}
			second_loop = 0;
			//if(voice_detected==1 && second_happen < tm->tm_sec)
			//{
			//	sound_duration++;
				//second_happen = tm->tm_sec;
			//}
			//else if(voice_detected==0)
			//{
			//	sound_duration=0;
			//}
			printf("Duration %d\n",sound_duration);
		}
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      //fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      //fprintf(stderr,
      //        "error from read: %s\n",
      //        snd_strerror(rc));
    } else if (rc != (int)frames) {
      //fprintf(stderr, "short read, read %d frames\n", rc);
    }
    //rc = write(1, buffer, size);
    //if (rc != size)
    //  fprintf(stderr,
    //          "short write: wrote %d bytes\n", rc);
	//printf("\n");
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  return 0;
}

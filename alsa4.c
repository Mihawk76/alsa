/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <time.h>
#include <math.h>

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
	int second_log[100000];
	int value_log[100000];
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
		sound_duration = 0;
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
		int d = 0;
		for(i = 0; i<=nArray;i++){
			if(buffer[i]>0){
				//printf("%d#",buffer[i]);
				int amp = (buffer[i])/div;
    		//for(d=0;d<buffer[i];d++)
    		for(d=0;d<amp;d++)
    		{   
      		printf("X");
    		}   
    		printf("|%d\n",buffer[i]);
				if(buffer[i] > 0 && buffer[i] < 200)
				{
					voice_detected = 1;	
					if(/*second_loop == 0 || (second_loop > 0 && second_log[second_loop-1] < (tm->tm_min*60 + tm->tm_sec))*/1)
					{
						second_log[second_loop] = (tm->tm_min*60 + tm->tm_sec);
						value_log[second_loop] = buffer[i];
						printf("\nsecond %d snd_loop %d\n",second_log[second_loop],second_loop);
						second_loop++;
						//if(second_happen == 0){
						//	second_happen = tm->tm_sec;
						//}
					}
				}
				else if((buffer[i]*buffer[i])<100)
				{
					voice_detected = 0;
				}
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
			printf("loop %d\n",second_loop);
			//if(second_loop >= 1000){
			//	second_loop = 0;
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
			int time_avg = 0;
			if(second_loop > 9000){
			for(d=1;d<(second_loop-1);d++)
				{
						if(second_log[d-1] < second_log[d]){
						float avg = 0.0, sum = 0.0, value = 0.0;
						int n_avg = 0;
						for(n_avg = 0;n_avg<(d-1);n_avg++)
						{
							value = value_log[n_avg];
							sum += value;
						}
						avg = sum / n_avg;
						if( avg > 70){
							if(time_avg == 0 || time_avg > 0 && time_avg < second_log[d]){
								sound_duration++;
								time_avg = second_log[d];
								printf("sd %d\n", sound_duration); 
							}
						}
					//if(value_log[d] > 100 && value_log[d-1] > 100
 				//	&& value_log[d] < 1000 && value_log[d-1] < 1000){
						//sound_duration = (second_log[d] - second_log[d-1]) + sound_duration;
						//printf("loop %d Duration %d\n", d, sound_duration);
						//printf("second 0 %d 1 %d\n",second_log[d-1],second_log[d]);
						//printf("value 0 %d 1 %d\n",value_log[d-1],value_log[d]);
						printf("loop %d second %d, value %d duration %d avg %f\n", d, second_log[d], value_log[d], sound_duration, avg);
					}
				}
			}
			if(second_loop > 10000){
				break;
			}
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  return 0;
}

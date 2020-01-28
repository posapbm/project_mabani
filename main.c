#include<stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "endian.h"
int note, tempo;
void readType(char type[], FILE* file); //reading type of header 
void readLength(int length, FILE* file); //reading length of header
void readFormat(int format, FILE* file); //reading format of header
int readTrack(int track, FILE* file);    // reading number of tracks in header
int readDivision(int division, FILE* file); //reading division from hedaer
void trackHeader(FILE* file);//reading track header and track length
int deltatime(FILE* file); //finding delta time of note on and note off
int eventtype(FILE* file); //finding the even which we need
double getfreq(int note); //replacing note with proper frequance
int main()
{
	
    FILE* file = fopen("Ed_Sheeran_Shape_of_You_midi_by.mid", "rb");
    char type[5];
    int length=0, track=0, format=0, division=0, tracklength=0, i=0, j = 0, Note=0, freq=0 , delta=0;
    int event=0;
    int flag=0;
    double lengthnote,notefreq;
    readType(type, file);
    readLength(length, file);
    readFormat(format, file);
    track=readTrack(track, file);
    division = readDivision(division, file);
    for(i=0;i<track;i++) // this loop is for the files which have more than one track
	{
    	trackHeader(file);
		
    do
    {
        delta = deltatime(file);
        flag = eventtype(file);
        
        if (flag == -1)
        {
        	printf("error"); //not finding any event
            exit(1);
        }
        if (flag == 2)
        {
            notefreq = getfreq(note);

            //lengthnote = (10000 * division * delta) / tempo;                    //type 1 for finding duration from temp and delta time
            int duration;                                                         //type 2 for finding duration from temp and delta time
            duration = (60*1000000) / tempo;

            duration *= division;

            lengthnote= 60000 / duration;

            lengthnote *= delta;
			lengthnote-=((lengthnote/100)*50);
            Beep(notefreq+30, lengthnote);
            
        }
        if(flag==-2)
		{
			("ending track");
		}
    }while(flag!=-2); // finding event ff2f
	}
	printf("\n\n\n\n");
	printf("THEEND");
//	fclose(infile);
	return(1);
    

}

void readType(char type[], FILE* file)
{

    fread(type, 4, 1, file);

    type[4] = '\0';
    printf("type: %s\n" , type);
}

void readLength(int length, FILE* file)
{
    fread(&length, 4, 1, file);
    length = changeEndian(length);
    printf("length: %d\n",length);
}

void readFormat(int format, FILE* file)
{
    unsigned char change[2];
    fread(change, 2, 1, file);
    format = change[1] | change[0] << 8;
    printf("format %d," , format);

    if (format == 2)
    {
        printf("eror");
        exit(1);
    }
}

int readTrack(int track, FILE* file)
{
    unsigned char change[2];
    fread(change, 2, 1, file);
    track = change[1] | change[0] << 8;
    printf(" %d tracks," , track);
    return track;
}

int readDivision(int division, FILE* file)
{
    unsigned char change[2];
    fread(change, 2, 1, file);
    division = change[1] | change[0] << 8;
    printf(" division: %d ticks / 1/4 note" , division);
    return division;
}


void trackHeader(FILE* file)
{
    char trackname[5];
    int tracklength;

    fread(trackname, 4, 1, file);

    trackname[4] = '\0';

    fread(&tracklength, 4, 1, file);
    tracklength = changeEndian(tracklength); // changing endian of track length;

    printf("\n%s\n%d\n", trackname, tracklength);
}

int deltatime(FILE* file) //masking for shifting and finding the code in stackoverflow
{

    register unsigned long value;
    register unsigned char c;

    if ((value = fgetc(file)) & 0x80) {
        value &= 0x7F;
        do {
            value = (value << 7) + ((c = getc(file)) & 0x7F);
        } while (c & 0x80);
    }


    return (value);

}
int eventtype(FILE* file)
{
    unsigned char event;
    unsigned int length=0;
    unsigned char type=0;
    unsigned char temp[4];
    unsigned int i=0;
    unsigned char marg[200];

    fread(&event, 1, 1, file);

    if (event >= 0x80 && event < 0x90) // note off
    {

        puts("find NOTE event");
        fread(&note, 1, 1, file);
        fread(marg, 1, 1, file);
        return 2;

    }
    else if(event >= 0x90 && event < 0xA0){ //note on

        puts("find NOTE event");
        fread(&note, 1, 1, file);
        fread(marg, 1, 1, file);

    }
    else if (event >= 0xA0 && event < 0xC0) //unimportant even
    {
        fread(marg, 1, 1, file);
        fread(marg, 1, 1, file);
    }
    else if (event >= 0xC0 && event < 0xE0) //unimportant even
    {
        fread(marg, 1, 1, file);
    }
    else if (event >= 0xE0 && event < 0xF0) //unimportant even
    {
        fread(marg, 1, 1, file);
        fread(marg, 1, 1, file);
    }
    else if (event == 0xF0 || event == 0xF7) // unimportant even
    {
        fread(&length, 1, 1, file);
        fread(marg, length, 1, file);
    }
    else if (event == 0xFF) // ff event
    {
        puts("find FF event");
        fread(&type, 1, 1, file);

        if (type == 0x51)              //tempo
        {
            fread(&length, 1, 1, file);

            for (i = 0; i < length; i++)
            {
                fread(temp, 1, 1, file);
            }
            tempo = temp[0] << 16 | temp[1] << 8 | temp[2];
           
        }
        else if (type == 0x2f)
        {
        	
            fread(marg, 1, 1, file);
            return -2;
        }
        else
        {
            fread(&length, 1, 1, file);
            fread(marg, length, 1, file);
        }
    }
    else
    {
        return -1;
    }
    return 5;
}
double getfreq(int note){

    float freq[] = {16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,16.35,27.50,29.14,30.87,32.70,34.65,36.71,38.89,41.20,43.65,46.25,49.00,51.91,55.00,58.27,61.74,65.41,69.30,73.42,77.78,82.41,87.31,92.50,98.00,103.83,110.00,116.54,123.47,130.81,138.59,146.83,155.56,164.81,174.61,185.00,196.00,207.65,220.00,233.08,246.94,261.63,277.18,293.66,311.13,329.63,349.23,369.99,392.00,415.30,440.00,466.16,493.88,523.25,554.37,587.33,622.25,659.25,698.46,739.99,783.99,830.61,880.00,932.33,987.77,1046.50,1108.7,1174.6,1244.5,1318.51,1396.91,1479.98,1567.98,1661.22,1760.00,1864.66,1975.53,2093.00,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520.00,3729.31,3951.07,4186.01,4434.92,4698.63,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,7040.00,7458.62,7902.13};
   
	return freq[note-12];
    

}


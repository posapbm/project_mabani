#include<stdio.h>
#include<Windows.h>

int note, tempo;

void readType(char type[],FILE *file);
void readLength(int length,FILE *file);
void readFormat(int format,FILE *file);
void readTrack(int track,FILE *file);
int readDivision(int division,FILE *file);
int changeEndian(int a);
void trackHeader (FILE *file , char trackname[] , int tracklength);
int deltatime(FILE *file);
int eventtype(FILE *file);

int main()
{
    FILE *file = fopen("morse_code_a.mid","rb");
    char type[4] , trackname[4];
	int length, format, division, tracklength,track, i, j = 0, a, Note , delta , lengthnote;
    int event;
	int flag=0;
    float freq, notefreq;
    readType(type,file);
	readLength(length,file);
	readFormat(format,file);
	readTrack(track,file);
	division = readDivision(division,file);
	trackHeader (file , trackname , tracklength);
	delta = deltatime(file);
	flag = eventtype(file);
	while (flag!=-2)
	{
	
		if (flag == -1)
		{
			exit(1);
		}
		if (flag == 2)
		{
			
			FILE* infile = fopen("midinote.txt", "r");
			
			fscanf(infile, "%d%f", &Note, &freq);
			
			printf("\n%d\n%d" , note , Note);

			while (Note != note)printf("x");
			{
				fscanf(infile, "%d%f", &Note, &freq);
			}
			
			notefreq = freq;

			lengthnote = (10000 * division * delta) / tempo;

			Beep(notefreq, lengthnote);
		}
		
		delta = deltatime(file);
		flag = eventtype(file);
		
		
	}
	
}

void readType(char type[],FILE *file)
{
    fread(type,4,1,file);
   
    //printf("type: %s\n" , type);
}

void readLength(int length,FILE *file)
{
    fread(&length,4,1,file);
    length = changeEndian(length);
    //printf("length: %d\n",length);
}

void readFormat(int format,FILE *file)
{
	unsigned char change[2];
    fread(change,2,1,file);
    format = change[1] | change[0] << 8;
    //printf("format %d," , format);
	
	if (format == 2)
	{
		printf("eror");
		exit(1);
	}
}

void readTrack(int track,FILE *file)
{
	unsigned char change[2];
    fread(change,2,1,file);
    track = change[1] | change[0] << 8;
    //printf(" %d tracks," , track);
}

int readDivision(int division,FILE *file)
{
	unsigned char change[2];
    fread(change,2,1,file);
    division = change[1] | change[0] << 8;
	//printf(" division: %d ticks / 1/4 note" , division);
	return division;
}

int changeEndian(int a) {
    return (((a>>24) & 0x000000ff | ((a>>8) & 0x0000ff00) | ((a<<8) & 0x00ff0000) | ((a<<24) & 0xff000000)));
}

void trackHeader (FILE *file , char trackname[] , int tracklength)
{
	int i;

	
		fread(trackname ,4,1,file);
	
		fread(&tracklength,4,1,file);
 	   tracklength = changeEndian(tracklength);
    
	    printf("%s\n%d" , trackname , tracklength);
	
	
    
    
}

int deltatime(FILE *file)
{
	int counter = 1, delta = 0;
    unsigned char deltaT;
	fread(deltaT, 1, 1, file);
	delta = deltaT << 8;
	
	while (deltaT >= 0x80 && counter <= 3)
	{
		fread(deltaT, 1, 1, file);
		delta |= deltaT << 8;
		counter++;
	}

	return delta;
    
   

}
int eventtype(FILE *file)
{
	char event;
	int length;
	char data[1000];
	char type;
	char temp[4];
	int i;
	int marg;
	int dada;

	fread(event,1,1,file);
	if (event >= 0x80 || event < 0xA0)
	{
		fread(dada, 1, 1, file);
		note = dada;
		fread(marg, 1, 1, file);

		return 2;
	}
	else if (event >= 0xA0 || event < 0xC0)
	{
		fread(marg, 1, 1, file);
		fread(marg, 1, 1, file);
	}
	else if (event >= 0xC0 || event < 0xE0)
	{
		fread(marg, 1, 1, file);
	}
	else if (event >= 0xE0 || event < 0xF0)
	{
		fread(marg, 1, 1, file);
		fread(marg, 1, 1, file);
	}
	else if (event == 0xF0 || event == 0xF7)
	{
		fread(length, 1, 1, file);
		fread(marg,length, 1, file);
	}
	else if (event == 0xFF)
	{
		fread(type, 1, 1, file);

		if (type == 0x51)              //tempo
		{
			fread(length, 1, 1, file);

			for (i = 0; i <length; i++)
			{
				fread(temp[i], 1, 1, file);
			}
			tempo = temp[0]<<16|temp[1]<<8|temp[2] ;
		}
		else if (type == 0x2f)
		{
			fread(marg, 1, 1, file);
			return -2;
		}
		else 
		{
			fread(length, 1, 1, file);
			fread(marg, length, 1, file);
		}
	}
	else
	{
		return -1;
	}
	return 5;
}


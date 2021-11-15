#include "source.h"
#include "gps.h"

int parse_comma_delimited_str(char *string, char **fields, int max_fields) {
	int i = 0;
	fields[i++] = string;

	while ((i < max_fields) && NULL != (string = strchr(string, ','))) {
		*string = '\0';
		fields[i++] = ++string;
	}

	return --i;
}

// 1      2         3 4          5 6           7 8     9  10      11 12 13
// $GPRMC,030501.00,A,3755.57351,S,14503.53532,E,0.300,  ,101121 ,  ,  ,A*6B
// total of 12 fields

int nmea_sentence_to_time(char *sentence, struct GpsTime *time) {
  char *field[20];
  
  // parse sentence
  int token_count = parse_comma_delimited_str(sentence, field, 20);

  //printf("Got %d tokens\n", token_count);
  
  // does sentence have 12 tokens?
  if(token_count == 12) {
    //printf("Sentence has 12 tokens\r\n");
    // does the first token match the expected value of $GPRMC
    if(strcmp(field[0], "$GPRMC") == 0) {
      //printf("Sentence matches expectations\r\n");
      // get the time field and split it
      char hour[3];
      char min[3];
      char sec[3];
      strncpy(hour, &field[1][0], 2);
      strncpy(min, &field[1][2], 2);
      strncpy(sec, &field[1][4], 2);
      hour[2] = '\0';
      min[2] = '\0';
      sec[2] = '\0';
      //printf("Time %s:%s:%s\r\n",hour, min, sec);
      // convert to ints and put into the struct
      int ihour = atoi(hour);
      int imin = atoi(min);
      int isec = atoi(sec);
      // put into struct to return
      struct GpsTime t;
      t.hour = ihour;
      t.minute = imin;
      t.second = isec;
      *time = t;
      return 1;
    } else {
      return 0;
    }
  } else {  
    return 0;
  }
}
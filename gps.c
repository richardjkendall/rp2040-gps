/*
 * File:   gps.c
 * Author: Richard Kendall
 * Version: 1
 * -----------------------
 * Methods to parse GPS NMEA sentences to get time information
 */

#include "source.h"
#include "gps.h"

/*
 * Function: parse_comma_delimited_str
 * -----------------------------------
 * Splits a string up by commas into an array of strings
 * 
 * string:     a pointer to the string to be split up
 * fields:     a pointer to the array of strings to be populated
 * max_fields: the maximum size of array of strings
 *
 * returns: an integer representing the number of fields found
 */
int parse_comma_delimited_str(char *string, char **fields, int max_fields) {
	int i = 0;
	fields[i++] = string;

	while ((i < max_fields) && NULL != (string = strchr(string, ','))) {
		*string = '\0';
		fields[i++] = ++string;
	}

	return --i;
}

/*
 * Function: nmea_sentence_to_time
 * -------------------------------
 * Converts the NMEA $GPRMC sentence and extracts the time, field=2 below
 * 
 * 1      2         3 4          5 6           7 8     9  10      11 12 13
 * $GPRMC,030501.00,A,3755.57351,S,14503.53532,E,0.300,  ,101121 ,  ,  ,A*6B
 * 
 * sentence: a pointer to the string containing the NMEA sentence
 * time:     a pointer to time struct
 *
 * returns: 1 if a time is found and 0 otherwise
 */
int nmea_sentence_to_time(char *sentence, struct GpsTime *time) {
  char *field[20];
    
  // parse sentence
  int token_count = parse_comma_delimited_str(sentence, field, 20);
  
  // does sentence have 12 tokens?
  if(token_count == 12) {
    // does the first token match the expected value of $GPRMC
    if(strcmp(field[0], "$GPRMC") == 0) {
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
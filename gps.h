struct GpsTime {
  int hour;
  int minute;
  int second;
};

int parse_comma_delimited_str(char *string, char **fields, int max_fields);
int nmea_sentence_to_time(char *sentence, struct GpsTime *time);
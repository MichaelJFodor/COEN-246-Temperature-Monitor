/*
	THIS PROGRAM:
		read temperature/humidity sensor with Raspberry Pi

		Read two sensors; if data are good, write to a db

		wiringPi library and mysql connector/C used

		-- search online and see gpiopi and wiringPi docs for help
		-- with some modification, this program could work with gpiopi library
		-- only tested with dht11 sensors
		-- should also work with dht22 sensor with some changes in delay and read frequency
			-- dht11: 1 read per 3 sec is ok
			-- dht22: 1 read per 5 sec or lower frequency
		-- use the given make file to compile, only tested with gcc, not tested with clang
		-- db info and queries are hard-coded, change accrodingly

	Big Thank You for all those people online and authors of gpiopi and wiringPi
*/

/*
	DHT11 / DHT22 data format:
		byte1: RH INT
		byte2: RH DECIMAL
		byte3: TEMP INT
		byte4: TEMP DECIMAL
		byte5: CHECKSUM
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <wiringPi.h>
#include <mysql/mysql.h>

#define DHTPIN_0	7
#define DHTPIN_1	0
#define MAXT		85

int dht11_dat_0[5] = {0, 0, 0, 0, 0}; // hold dht11 sensor data 40 bit, last byte is checksum
int dht11_dat_1[5] = {0, 0, 0, 0, 0};

void read_dht11() {
	// prepare DB
	MYSQL *conn = mysql_init(NULL);
	if(conn == NULL) {
		printf("fail to init connection.\n");
		mysql_close(conn);
		exit(1);
	}

	// connect to DB
	if(mysql_real_connect(conn, "coen243.ccj2rgp4lzvc.us-west-1.rds.amazonaws.com", 
		"group_member", "coen243", "coen243_project", 0, NULL, 0) == NULL) {
		printf("fail to establish DB real connection.\n");
		mysql_close(conn);
		exit(1);
	}

	/*
		SENSOR 1 READ
	*/
	
	uint8_t laststate = HIGH;
	uint8_t cnt = 0;
	uint8_t bit_cnt = 0;

	dht11_dat_0[0] = dht11_dat_0[1] = dht11_dat_0[2] = dht11_dat_0[3] = dht11_dat_0[4] = 0;

	// pull down pin for 18 ms
	pinMode(DHTPIN_0, OUTPUT);
	digitalWrite(DHTPIN_0, LOW);
	delay(18);
	// pull up pin for 40us
	digitalWrite(DHTPIN_0, HIGH);
	delayMicroseconds(40);
	// pin ready for read, to INPUT mode
	pinMode(DHTPIN_0, INPUT);

	// read data when pin changed
	for(uint8_t i = 0; i < MAXT; i++) {
		cnt = 0;
		// no reading, keep trying
		while(digitalRead(DHTPIN_0) == laststate) {
			cnt++;
			delayMicroseconds(2); // this number should be changed if program not working {1,2,3,4,5} all possible
			if(cnt == 255) {
				break;
			}
		}

		laststate = digitalRead(DHTPIN_0);

		if(cnt == 255) {
			break;
		}

		// some reads discard, could be changed
		if((i >= 4) && (i % 2 == 0)) {
			dht11_dat_0[bit_cnt / 8] <<= 1; // shift bit to its position
			// this check could also be changed if program not working {16-50} all possible
			if(cnt > 16) {
				dht11_dat_0[bit_cnt / 8] |= 1;
			} 
			bit_cnt++;
		}
	}

	// check the 40 bit data, check checksum, if good write to db
	if((bit_cnt >= 40) &&
		dht11_dat_0[4] == ((dht11_dat_0[0] + dht11_dat_0[1] + dht11_dat_0[2] + dht11_dat_0[3]) & 0xFF)) {
		float rh = dht11_dat_0[0] + dht11_dat_0[1] / 10.0;
		float temp = dht11_dat_0[2] + dht11_dat_0[3] / 10.0;
		char SQLString[64];
		printf("sensor 1: Humidity: %2.1f %%, Temperature: %2.1f C.\n", rh, temp);
		sprintf(SQLString,"INSERT INTO sensor1 VALUES(now(),%2.1f,%2.1f)", temp, rh);
		if (mysql_query(conn, SQLString)) {
			printf("sensor 1: query fail.\n");
			mysql_close(conn);
			exit(1);
		}
		printf("sensor 1: succeed.\n");
	}
	else {
		printf("sensor 1: bad data.\n");
	}

	/*
		SENSOR 2 READ
	*/

	laststate = HIGH;
	cnt = 0;
	bit_cnt = 0;

	dht11_dat_1[0] = dht11_dat_1[1] = dht11_dat_1[2] = dht11_dat_1[3] = dht11_dat_1[4] = 0;

	// pull down pin for 18 ms
	pinMode(DHTPIN_1, OUTPUT);
	digitalWrite(DHTPIN_1, LOW);
	delay(18);
	// pull up pin for 40us
	digitalWrite(DHTPIN_1, HIGH);
	delayMicroseconds(40);
	// pin ready for read, to INPUT mode
	pinMode(DHTPIN_1, INPUT);

	// read data when pin changed
	for(uint8_t i = 0; i < MAXT; i++) {
		cnt = 0;
		// no reading, keep trying
		while(digitalRead(DHTPIN_1) == laststate) {
			cnt++;
			delayMicroseconds(2); // this number should be changed if program not working {1,2,3,4,5} all possible
			if(cnt == 255) {
				break;
			}
		}

		laststate = digitalRead(DHTPIN_1);

		if(cnt == 255) {
			break;
		}

		// some reads discard, could be changed
		if((i >= 4) && (i % 2 == 0)) {
			dht11_dat_1[bit_cnt / 8] <<= 1; // shift bit to its position
			// this check could also be changed if program not working {16-50} all possible
			if(cnt > 16) {
				dht11_dat_1[bit_cnt / 8] |= 1;
			} 
			bit_cnt++;
		}
	}

	// check the 40 bit data, check checksum, if good write to db
	if((bit_cnt >= 40) &&
		dht11_dat_1[4] == ((dht11_dat_1[0] + dht11_dat_1[1] + dht11_dat_1[2] + dht11_dat_1[3]) & 0xFF)) {
		float rh = dht11_dat_1[0] + dht11_dat_1[1] / 10.0;
		float temp = dht11_dat_1[2] + dht11_dat_1[3] / 10.0;
		char SQLString[64];
		printf("sensor 2: Humidity: %2.1f %%, Temperature: %2.1f C.\n", rh, temp);
		sprintf(SQLString,"INSERT INTO sensor2 VALUES(now(),%2.1f,%2.1f)", temp, rh);
		if (mysql_query(conn, SQLString)) {
			printf("sensor 2: query fail.\n");
			mysql_close(conn);
			exit(1);
		}
		printf("sensor 2: succeed.\n");
	}
	else {
		printf("sensor 2: bad data.\n");
	}

	printf("\n");

	mysql_close(conn);
}

int main() {
	printf("start record data.\n\n");

	if(wiringPiSetup() == -1) {
		printf("setup error\n");
		exit(1);
	}

	while(1) {
		read_dht11();
		delay(5000); // refresh rate, this could be changed
	}

	return 0;
}

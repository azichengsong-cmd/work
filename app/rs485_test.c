/*
 * RS485 / UART test program for i.MX6ULL (UART3 -> /dev/ttymxc2)
 *
 * Usage:
 *   ./rs485_test /dev/ttymxc2 0    # send "hello rs485!" every 1s
 *   ./rs485_test /dev/ttymxc2 1    # receive and print
 *
 * Build:
 *   arm-linux-gnueabihf-gcc rs485_test.c -o rs485_test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

static int set_opt(int fd, int nSpeed)
{
	struct termios newtio;

	if (tcgetattr(fd, &newtio) != 0) {
		perror("tcgetattr");
		return -1;
	}

	cfmakeraw(&newtio);
	newtio.c_cflag |= (CLOCAL | CREAD);
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;
	newtio.c_cflag &= ~PARENB;
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_cflag &= ~CRTSCTS;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	switch (nSpeed) {
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 19200:
		cfsetispeed(&newtio, B19200);
		cfsetospeed(&newtio, B19200);
		break;
	case 38400:
		cfsetispeed(&newtio, B38400);
		cfsetospeed(&newtio, B38400);
		break;
	case 57600:
		cfsetispeed(&newtio, B57600);
		cfsetospeed(&newtio, B57600);
		break;
	case 115200:
	default:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	}

	tcflush(fd, TCIFLUSH);

	if (tcsetattr(fd, TCSANOW, &newtio) != 0) {
		perror("tcsetattr");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int fd, mode, ret;
	char buf[64];

	if (argc < 3) {
		printf("Usage: %s <dev> <0:send 1:recv> [baudrate]\n", argv[0]);
		printf("Example: %s /dev/ttymxc2 0 115200\n", argv[0]);
		return -1;
	}

	mode = atoi(argv[2]);

	fd = open(argv[1], O_RDWR | O_NOCTTY);
	if (fd < 0) {
		printf("open %s failed: %s\n", argv[1], strerror(errno));
		return -1;
	}

	if (set_opt(fd, argc > 3 ? atoi(argv[3]) : 115200) < 0) {
		close(fd);
		return -1;
	}

	printf("open %s success, mode=%d\n", argv[1], mode);

	if (mode == 0) {
		const char *msg = "hello rs485!\r\n";

		while (1) {
			ret = write(fd, msg, strlen(msg));
			if (ret < 0)
				printf("write failed: %s\n", strerror(errno));
			else
				printf("send: %s", msg);
			sleep(1);
		}
	} else {
		while (1) {
			memset(buf, 0, sizeof(buf));
			ret = read(fd, buf, sizeof(buf) - 1);
			if (ret > 0) {
				printf("recv: ");
				fwrite(buf, 1, ret, stdout);
				printf("\n");
			} else if (ret < 0) {
				printf("read failed: %s\n", strerror(errno));
			}
		}
	}

	close(fd);
	return 0;
}

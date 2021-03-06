/*
 * WebSocketClient.c
 *
 *  Created on: 2019年5月20日
 *      Author: cc
 *
 *                .-~~~~~~~~~-._       _.-~~~~~~~~~-.
 *            __.'              ~.   .~              `.__
 *          .'//                  \./                  \\`.
 *        .'//                     |                     \\`.
 *      .'// .-~"""""""~~~~-._     |     _,-~~~~"""""""~-. \\`.
 *    .'//.-"                 `-.  |  .-'                 "-.\\`.
 *  .'//______.============-..   \ | /   ..-============.______\\`.
 *.'______________________________\|/______________________________`.
 *.'_________________________________________________________________`.
 * 
 * 请注意编码格式
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "WebSocketClient.h"
#include "LxWebSocketClient.h"
#include "libwebsockets.h"

#define RX_BUFFER_SIZE (1024*200)

static struct lws *_wsiclient = NULL;
static WebSocketClient *_webSocketClient = NULL;

static int _webSockServiceCallback(struct lws *wsi,
		enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	WebSocketClient *webSocketClient = (WebSocketClient*) user;
	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		printf("INIT %p\n", wsi);

		break;
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		printf("LWS_CALLBACK_CLIENT_CONNECTION_ERROR %p\n", wsi);
		break;
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		printf("LWS_CALLBACK_CLIENT_ESTABLISHED %p\n", wsi);
		_webSocketClient = webSocketClient;
		webSocketClient->wsi = wsi;
		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:
		printf("LWS_CALLBACK_CLIENT_RECEIVE\n");
		break;
	case LWS_CALLBACK_CLIENT_WRITEABLE:
		printf("LWS_CALLBACK_CLIENT_WRITEABLE\n");
		_webSocketClient->lastWrTime = time(NULL);

		{
			char data[LWS_PRE + 100] = { 0 };
			int ret;

			printf("send->\n");
			memset(data + LWS_PRT, 'C', 100);
			lws_set_timeout(wsi, NO_PENDING_TIMEOUT, 0);
			ret = lws_write(wsi, (unsigned char *) (data + LWS_PRE), 100, LWS_WRITE_BINARY);

			if (100 != ret) {
				printf("lws_write waring  slen!= len\n");
				return -1;
			}

			if (lws_send_pipe_choked(wsi)) { //当发送阻塞，20秒超时处理
				printf("lws_send_pipe_choked  .......\n");
				lws_set_timeout(wsi, PENDING_TIMEOUT_SENT_CLIENT_HANDSHAKE, 20);
			}
		}

		break;
	case LWS_CALLBACK_CLOSED:
		printf("LWS_CALLBACK_CLOSED %p \n", wsi);
		break;
	case LWS_CALLBACK_WSI_DESTROY:
		printf("LWS_CALLBACK_WSI_DESTROY %p \n", wsi);
		{
			if (_webSocketClient == webSocketClient) {
				_webSocketClient = NULL;
				_wsiclient = NULL;
			}
			if (!webSocketClient) {

			}
		}
		break;
	default:
		break;
	}
	return 0;
}

static struct lws_protocols protocols[] = { { "myproto",
		_webSockServiceCallback, sizeof(WebSocketClient), 1024 * 100 }, { NULL,
NULL, 0, 0 } /* end */
};

void client_loop() {
	struct lws_context_creation_info info;
	struct lws_context *context = NULL;
	struct lws_client_connect_info i;

	memset(&info, 0, sizeof info);

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.iface = NULL;
	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;
	info.ssl_private_key_filepath = NULL;
	info.extensions = NULL; 	//lws_get_internal_extensions();
	info.gid = -1;
	info.uid = -1;
	info.options = 0;

	context = lws_create_context(&info);

	memset(&i, 0, sizeof(i));

	i.path = "/";
	i.address = "192.168.0.88"; //192.168.0.81
	i.port = 10086; //
	i.context = context;
	i.ssl_connection = 0;
	i.host = "hostname-cn";  //如果有证书，这里设定证书的CN名称
	i.origin = i.address;
	i.ietf_version_or_minus_one = -1;
	i.client_exts = NULL;
	i.protocol = "myproto";

	time_t tmstart = time(NULL);
	_wsiclient = lws_client_connect_via_info(&i);

	printf("_wsiclient=%p\n", _wsiclient);

	while (1) {
		if (_wsiclient == NULL && abs(time(NULL) - tmstart) > 5) {
			printf("need restart connect ....\n");
			tmstart = time(NULL);
			_wsiclient = lws_client_connect_via_info(&i);
		}
#if 0
		if (_webSocketClient) {
			if (_webSocketClient->lastWrTime > 0) {
				if (abs(time(NULL) - _webSocketClient->lastWrTime) > 10) {
					//如果10秒还没有发送数据,则关闭
					logd("need close ......\n");
					shutdown(lws_get_socket_fd(_webSocketClient->wsi), SHUT_RD);
				}
			}
		}
#endif
		lws_service(context, 200);
	}

	lws_context_destroy(context);

}


int main(int argc, char **argv) {
	client_loop();
	return 0;
}

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define __USE_GNU
#include <dlfcn.h>

int socket(int domain, int type, int protocol) {

  // get the old function
  static int (*socket_real)(int, int, int) = NULL;
  if (!socket_real) socket_real = dlsym(RTLD_NEXT, "socket");

  printf("domain:%d type:%d protocol:%d\n", domain, type, protocol);

  if ((domain == AF_INET) && (type == SOCK_STREAM)) {
    return(socket_real(AF_UNIX, SOCK_STREAM, 0));
  } else {
    return(socket_real(domain, type, protocol));
  }

}

int bind(int socket, const struct sockaddr *address, socklen_t address_len) {

  // get the old function
  static int (*bind_real)(int, const struct sockaddr *, socklen_t) = NULL;
  if (!bind_real) bind_real = dlsym(RTLD_NEXT, "bind");

  char *binding_port = getenv("PORT");
  char *socket_path  = getenv("PORT_BINDER_SOCKET");

  if (binding_port == NULL)
    binding_port = 0;

  if (socket_path == NULL)
    socket_path = "/tmp/port-binder.sock";

  if (address->sa_family == AF_INET) {
    struct sockaddr_in *address_in = (struct sockaddr_in *)address;
    struct sockaddr_un *address_un = (struct sockaddr_un *)malloc(sizeof(struct sockaddr_un));

    printf("family:%d addr:%s port:%d binding_port:%s\n", address_in->sin_family, inet_ntoa(address_in->sin_addr), ntohs(address_in->sin_port), binding_port);

    if (ntohs(address_in->sin_port) == atoi(binding_port)) {
      address_un->sun_family = AF_UNIX;
      strcpy(address_un->sun_path, socket_path);
      printf("going for it\n");
      return(bind_real(socket, (struct sockaddr *)address_un, strlen(address_un->sun_path)+sizeof(address_un->sun_family)));
    }
    free(address_un);
  }

  return(bind_real(socket, address, address_len));

}

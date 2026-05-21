#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

/* ════════════════════════════════════════
   Fonction chats — même logique que serveur
   ════════════════════════════════════════ */
void chats(int D) {
    char psl[BUF_SIZE];  /* pseudo local   */
    char psd[BUF_SIZE];  /* pseudo distant */
    char me[BUF_SIZE];   /* message envoyé */
    char mr[BUF_SIZE];   /* message reçu   */

    memset(psl, 0, BUF_SIZE);
    memset(psd, 0, BUF_SIZE);
    memset(me,  0, BUF_SIZE);
    memset(mr,  0, BUF_SIZE);

    /* Saisir pseudo et échanger */
    printf("Saisir votre pseudo : ");
    fflush(stdout);
    fgets(psl, BUF_SIZE, stdin);
    psl[strcspn(psl, "\n")] = '\0';

    send(D, psl, strlen(psl) + 1, 0);
    recv(D, psd, BUF_SIZE, 0);
    printf("Pseudo distant : %s\n", psd);

    /* Tant que (mr != "quitter") et (me != "quitter") */
    while (strcmp(mr, "quitter") != 0 && strcmp(me, "quitter") != 0) {
        memset(me, 0, BUF_SIZE);
        memset(mr, 0, BUF_SIZE);

        /* Recv d'abord (le client reçoit en premier) */
        int n = recv(D, mr, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Connexion fermée.\n");
            break;
        }
        printf("[%s] : %s\n", psd, mr);

        if (strcmp(mr, "quitter") == 0) break;

        /* Puis envoie */
        printf("[%s] : ", psl);
        fflush(stdout);
        fgets(me, BUF_SIZE, stdin);
        me[strcspn(me, "\n")] = '\0';

        send(D, me, strlen(me) + 1, 0);
    }

    close(D);
}

/* ════════════════════════════════════════
   Main — socket, connect, puis chats()
   ════════════════════════════════════════ */
int main(int argc, char *argv[]) {
    int desc;
    struct sockaddr_in addrl;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    /* Création du socket */
    desc = socket(AF_INET, SOCK_STREAM, 0);
    if (desc < 0) {
        perror("Erreur socket");
        exit(1);
    }

    /* Configuration adresse serveur */
    memset(&addrl, 0, sizeof(addrl));
    addrl.sin_family = AF_INET;
    addrl.sin_port   = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addrl.sin_addr);

    /* Connexion */
    if (connect(desc, (struct sockaddr *)&addrl, sizeof(addrl)) < 0) {
        perror("Erreur connect");
        exit(1);
    }

    printf("Connecté au serveur %s:%s\n", argv[1], argv[2]);

    chats(desc);

    return 0;
}
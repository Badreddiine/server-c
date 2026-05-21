#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define NBR_MAX  5

/* ════════════════════════════════════════
   Fonction chats — gère la communication
   avec un client (comme dans l'algorithme)
   ════════════════════════════════════════ */
void chats(int D) {
    char psl[BUF_SIZE];  /* pseudo local   */
    char psd[BUF_SIZE];  /* pseudo distant */
    char me[BUF_SIZE];   /* message envoyé */
    char mr[BUF_SIZE];   /* message reçu   */

    /* Initialisation */
    memset(psl, 0, BUF_SIZE);
    memset(psd, 0, BUF_SIZE);
    memset(me,  0, BUF_SIZE);
    memset(mr,  0, BUF_SIZE);

    /* Saisir pseudo local et échanger */
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

        /* Afficher psl, lire me */
        printf("[%s] : ", psl);
        fflush(stdout);
        fgets(me, BUF_SIZE, stdin);
        me[strcspn(me, "\n")] = '\0';

        /* Send(D, me) */
        send(D, me, strlen(me) + 1, 0);

        /* Recv(D, mr) */
        int n = recv(D, mr, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Connexion fermée.\n");
            break;
        }

        /* Afficher psd, mr */
        printf("[%s] : %s\n", psd, mr);
    }

    close(D);
}

/* ════════════════════════════════════════
   Main — création socket, bind, listen,
   accept, puis appel chats()
   ════════════════════════════════════════ */
int main(int argc, char *argv[]) {
    int desc, desc_client;
    struct sockaddr_in addrl, addr_distant;
    socklen_t taille;

    /* Récupération numéro port */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    addrl.sin_port = htons(atoi(argv[1]));

    /* Création du socket */
    desc = socket(AF_INET, SOCK_STREAM, 0);
    if (desc < 0) {
        perror("Erreur socket");
        exit(1);
    }

    /* Configuration de l'adresse locale */
    memset(&addrl, 0, sizeof(addrl));
    addrl.sin_family      = AF_INET;
    addrl.sin_addr.s_addr = INADDR_ANY;
    addrl.sin_port        = htons(atoi(argv[1]));

    /* Bind */
    int en = bind(desc, (struct sockaddr *)&addrl, sizeof(addrl));
    if (en != 0) {
        perror("Erreur bind");
        exit(1);
    }

    /* Listen */
    en = listen(desc, NBR_MAX);
    if (en != 0) {
        perror("Erreur listen");
        exit(1);
    }

    printf("Serveur en attente sur le port %s...\n", argv[1]);

    /* Tant que vrai — accept et appel chats */
    while (1) {
        taille = sizeof(addr_distant);
        desc_client = accept(desc, (struct sockaddr *)&addr_distant, &taille);

        if (desc_client > 0) {
            printf("Client connecté : %s:%d\n",
                   inet_ntoa(addr_distant.sin_addr),
                   ntohs(addr_distant.sin_port));

            chats(desc_client); /* appel fonction chats */
        }
    }

    close(desc);
    return 0;
}

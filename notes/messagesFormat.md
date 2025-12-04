
# Fromat d'un message IRC :

[ ":" <prefix> <SP> ] <command> [params] <CRLF>

- Prefix --------------------------------------

Le prefix n'est present QUE quand server -> client
Il ne dois pas y avoir de prefix quand client -> server

Deux types de prefix : 

  1/ Server envoi message systeme 
    le prefix se compose des deux points et du nom du server suivit d'un espace

    ex: ":42server " 

  2/ Server diffuse message d'un client 
    le prefix se compose de trois elements toujours dans le meme ordre : 
    - le nickname preceder de deux points
    - le username preceder d'un point d'exclamation 
    - le hostname preceder d'un @

    Imaginons qu'un client se register en replissant Nick = bob, USER = bobby et Hostname = 127.0.0.1
    prefixe = ":bob!bobby@127.0.0.1 "


- Command -------------------------------------

Les commandes ne sont pas sensibles a la case 
NICK, Nick, nick, NiCk => c'est la meme commande

1/ Commandes d'enregistrement : 

PASS	Fournit le mot de passe du serveur
NICK	Declarer le nickname
USER	Declarer le username

2/ Commande de messagerie :

PRIVMSG	Envoi de message prive ou dans un canal
NOTICE	Comme PRIVMSG mais sans erreurs renvoyees

3/ Commande de gestion de channel

JOIN	  Rejoindre ou creer un canal
PART	  Quitter un canal
TOPIC	  Lire ou definir le topic
MODE	  Lire ou definir des modes du canal
KICK	  Ejecter un utilisateur d’un canal
INVITE	Inviter un utilisateur dans un canal

4/ Commandes protocolaires

PING	Le serveur demande : es-tu vivant ?
PONG	Le client repond
QUIT	Deconnexion normale
ERROR	Rare, emis par le serveur si gros probleme

- Params --------------------------------------

<middle> <middle> ... [:<trailing>]

Deux types de params : 

1/ Middle parameters
- Pas d'espace DANS un middle param 
- Chaque middle params doit etre separer par un espace 
- Ne commence PAS par ':'
- Peut contenir lettres, chiffres, #, &, =, -, _, etc

2/ Trailing parameter

le trailing parameter peut contenir des espaces donc c'est celui utiliser
pour les messages text

- Commence par ':'
- Tout ce qui suit ces ':' devient un seul parametre
- Peut contenir des espaces
- Peut contenir n'importe quels type de caractere saud CRLF

EX : PRIVMSG #42 :hello world!
PRIVMSG       -> COMMAND
#42           -> Middle parameter
hello world!  -> trailing parameter

EX : :Bob!u@localhost PRIVMSG #general :Salut tout le monde !
Bob!u@localhost         -> Prefix
PRIVMSG                 -> COMMAND
#general                -> Middle parameter
Salut tout le monde !   -> trailing parameter



## ATTENTION

- 15 params MAX par commande selon RFC

- 512 octets max par message, CRLF inclut dans les deux sens (client -> server et server -> client);
  si client envoit un message trop long : 
  - soit trunc le message (couper en bout de 510 octets + CRLF)
  - soit erreur + fermeture de connection
  - soit ignorer le message
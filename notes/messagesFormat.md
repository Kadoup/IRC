
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

- Params --------------------------------------

Peer-to-Peer Applications

P2P Application 

In this project,a P2P (Peer-to-Peer) application will be developed.The application consists of an index server and a number of peers. Under this application, peers can exchange content among themselves through the support of the index server. A peer that has a piece of content(a movie,a songor a text file) available for download by other peers is called the content server ofthat content. Similarly, a peer that wants to download a piece of contentis called the content client of that content. A peer can be both a content server of a set of content and a content client of another set of content. The content server registers its content to the index server. In turn, the content client finds the address of a content server from the index server. The communication between the index server and a peer is based on UDP while the content download is based on TCP. The following figure illustrates the P2P mechanism.

<TeXmacs|1.0.7.2>

<style|generic>

<\body>
  <doc-data|<doc-title|Langage C<next-line><with|font-base-size|8|<em|Micro-projet
  n<degreesign>41 : Cahier des charges>><next-line><with|font-base-size|6|<em|Montagnes
  fractales>>>|<doc-author-data|<author-name|Fran�ois-Xavier Thomas>>|dd>

  \;

  Il est demand� de r�aliser un programme capable de construire un paysage
  fractal tridimensionnel. Son �x�cution se d�roulera en deux temps et devra
  :

  <\enumerate-numeric>
    <item>G�n�rer le paysage gr�ce � une m�thode de subdivision r�cursive en
    ajoutant une composante al�atoire d'amplitude donn�e � chaque it�ration

    <item>L'afficher suivant des param�tres comme : position du soleil,
    position de la cam�ra, ...
  </enumerate-numeric>

  Les couleurs des diff�rentes facettes du paysage devront �tre reproduites
  de mani�re r�aliste (neiges �ternelles, plaines, ...).

  <section|1. Fonctionnalit�s du logiciel>

  Le logiciel devra �tre capable de construire un paysage avec un aspect
  naturel. Il devra pouvoir :

  <\itemize-dot>
    <item>Construire un paysage fractal, et de le reconstruire � la demande
    de l'utilisateur

    <item>Changer � la vol�e les diff�rents param�tres (position de
    l'observateur, soleil, couleur, etc...) avec une interface graphique

    <item>Enregistrer l'image dans un fichier

    <item>Enregistrer les param�tres dans un fichier
  </itemize-dot>

  <section|2. Interface avec l'utilisateur>

  Les param�tres du logiciel seront :

  <\itemize>
    <item>Position de l'observateur : � r�gler au clavier et avec des champs
    de saisie

    <item>Position du soleil : champs de saisie

    <item>Couleur de la lumi�re du soleil : champs de saisie

    <item>Couleurs du terrain suivant l'altitude : fichiers s�par�s et liste
    des fichiers du r�pertoire courant dans l'interface graphique

    <item>Profil des amplitudes de la variable al�atoire en fonction des
    it�rations : fichiers s�par�s et liste des fichiers du r�pertoire courant
    dans l'interface graphique
  </itemize>

  Le logiciel fournira ensuite en sortie le rendu du paysage demand�, avec
  possibilit� d'en enregistrer l'image dans un fichier JPEG.

  Le fichier <strong|indiquant la couleur du terrain> (avec pour extension
  .tc) aura cette forme :

  <\verbatim>
    <\with|par-par-sep|0.0666fn>
      n

      color_default

      min_1 max_1 color_1

      min_2 max_2 color_2

      ...

      min_n max_n color_n
    </with>
  </verbatim>

  O� <math|n> est le nombre de couleurs diff�rentes, <math|min<rsub|n>>est
  l'altitude minimum, <math|max<rsub|n>>l'altitude maximum,
  <math|color<rsub|n>> la couleur du terrain � cette altitude, et
  <math|color<rsub|default>> la couleur par d�faut du terrain. Les
  intervalles d'altitudes doivent �tre disjoints et tri�s par altitude
  croissante. Les couleurs seront indiqu�es sous la forme <verbatim|rrr ggg
  bbb> avec <math|R>, <math|G>, <math|B> les intensit�s respectives de rouge,
  vert et bleu de la couleur, de 0 � 255 en base 10.

  Le fichier <strong|indiquant le profil de hauteur de la variable al�atoire>
  (extension .rvp) aura la forme :

  <\verbatim>
    <\with|par-par-sep|0.0666fn>
      n

      h_1

      ...

      h_n
    </with>
  </verbatim>

  <math|n> indique le nombre d'it�rations et
  <math|(h<rsub|k>)<rsub|k\<in\>[\|1,n\|]>> le facteur correctif par lequel
  multiplier la variable al�atoire de base.
</body>

<\initial>
  <\collection>
    <associate|sfactor|2>
  </collection>
</initial>

<\references>
  <\collection>
    <associate|auto-1|<tuple|1|?>>
    <associate|auto-2|<tuple|2|?>>
    <associate|auto-3|<tuple|1|?>>
  </collection>
</references>

<\auxiliary>
  <\collection>
    <\associate|toc>
      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|1.
      Fonctionnalit�s du logiciel> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-1><vspace|0.5fn>

      <vspace*|1fn><with|font-series|<quote|bold>|math-font-series|<quote|bold>|2.
      Interface avec l'utilisateur> <datoms|<macro|x|<repeat|<arg|x>|<with|font-series|medium|<with|font-size|1|<space|0.2fn>.<space|0.2fn>>>>>|<htab|5mm>>
      <no-break><pageref|auto-2><vspace|0.5fn>
    </associate>
  </collection>
</auxiliary>
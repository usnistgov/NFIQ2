CREATED BY: MDI


The Makefile in this directory builds 3 versions of "minexv":

  minexv  - For use with MINEX04 generated templates
  minexov - For use with Ongoing MINEX generated templates
  minex2v - For use with MINEXII generated templates

Q: Why are there now 3 minex validation programs?

A: Note that previously there was only a 'minexv', which used the -p option to
enforce the Ongoing MINEX rules.  However, even with the -p some things weren't
being enforced.  Along came MINEX II with even more differences, so I decided
to split everything into 3 distinct executables built from the same source.
The -p option is still accepted in all cases, but is essentially useless in
the 'minexov' and 'minex2v' cases, since the semantics of that option are
always turned ON internally.  (It should'nt be used in the 'minexv' case, though
there's nothing preventing this)

Note, the first two are built statically linked with the 'libfmr' code, whereas
'minex2v' is dynamically linked.  (so to run minex2v 'libfmr' must be in the
dynamic loader's search path)


Major differences between what minexv, minexov and minex2v has to enforce:

MINEX04
  MAX RECORD LEN = 4500
  CBEFF PID (BOTH OWNER AND TYPE) MUST be 0
  IMPRESSION TYPE MAY = 0,1,2 or 3
  FINGER QUALITY USES 1,25,50,75,100 SCALE
  MINUTIAE QUALITY VALS MUST = 0

OMINEX
  MAX RECORD LEN = 800                         <CHANGE FROM PREV>
  CBEFF PID (BOTH OWNER AND TYPE) MUST be 0
  FINGER QUALITY USES 20,40,60.80,100 SCALE    <CHANGE FROM PREV>
  IMPRESSION TYPE MAY = 0 or 2                 <CHANGE FROM PREV>
  MINUTIAE QUALITY VALS MUST = 0

MINEX2
  MAX RECORD LEN = 800                         
  CBEFF PID (BOTH OWNER AND TYPE) MUST be > 0  <CHANGE FROM PREV>
  IMPRESSION TYPE MAY = 0 or 2
  FINGER QUALITY USES 20,40,60.80,100 SCALE
  MINUTIAE QUALITY VALS MAY >= 0               <CHANGE FROM PREV>

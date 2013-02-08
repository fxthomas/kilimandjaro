#include <stdio.h>

int main () {
   int i = 1, x = 16;
   FILE* f = fopen ("random_variable", "w+");
   fprintf (f, "%d\n", x);
   for (i = 1; i <= x; i++) {
      double xx = (double)i;
      fprintf (f, "%lf\n", 4000./(xx*xx));
   }
   fclose (f);
   return 0;
}

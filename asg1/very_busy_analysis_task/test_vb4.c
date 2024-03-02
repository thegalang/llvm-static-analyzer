/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
   int a, b, x, y, w, z;

   if (a > b) {
     x = b - a;
     y = a + b;
   }
   else if(a < b) {
    x = a + b;
    if(a * 2 > b) {
      y = b - a;
      return 0;
    }
   } else {

    w = a + b;
    z = b - a;
    x = b * a;
   }

   y = w + z;
}


using namespace std;

int lowerbound2(vector<Test> tests, int width){
   int alph;
   int lowerbound = 0;

   for(alph = 0 ; alph < width/2 ; alph++){
       int partA = 0;
       int partB = 0;
       int partC = 0;
       for(int i = 0; i < tests.size(); i++){
           if((width >= tests[i].TAM_width)&&(tests[i].TAM_width > width/2))
               partA += tests[i].length;
           else if(((w/2)>= tests[i].TAM_width)&&(tests[i].TAM_width >= alph))
               partB += tests[i].TAM_width * tests[i].length;
           else if(((width - alph)>= tests[i].TAM_width)&&(tests[i].TAM_width > (width/2)))
               partC += (width - tests[i].TAM_width)*tests[i].TAM_width;
       }

       lowerbound = max(lowerbound, (a + max(0, (b-c)/width)));
   }

   return lowerbound;
}

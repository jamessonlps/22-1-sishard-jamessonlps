/* Implementar função solucao_ex3 */
void solucao_ex3(long a, long b, int *c, int *d, int *e) {
    char retval = (a < b);
    *c = retval;
    retval = (a == b);
    *d = retval;
    retval = (a > b);
    *e = retval;
}

#include "../common/tester.h"
#include "mon_projet.h"

static void	t_exemple(void)
{
	CHECK(ma_fonction(2) == 4, "ma_fonction(2) = %d, attendu 4", ma_fonction(2));
	CHECK(ma_fonction(0) == 0, "ma_fonction(0) = %d, attendu 0", ma_fonction(0));
}

int	main(void)
{
	print_title("Mon projet");
	run_test("ma_fonction", t_exemple);
	return (print_summary());
}

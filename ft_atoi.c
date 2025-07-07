static int	white_spaces(char charsy)
{
	if (charsy == ' ' || charsy == '\t' || charsy == '\n' || charsy == '\v')
		return (1);
	else if (charsy == '\f' || charsy == '\r')
		return (1);
	else
		return (0);
}

static int	sign_control(char charsy)
{
	if (charsy == '-')
		return (-1);
	else if (charsy == '+')
		return (1);
	else
		return (0);
}

int	ft_atoi(const char *str)
{
	int	res;
	int	sign;
	int	i;

	res = 0;
	sign = 1;
	i = 0;
	while (white_spaces(str[i]))
		i++;
	if (sign_control(str[i]) == -1 || sign_control(str[i]) == 1)
	{
		sign = sign_control(str[i]);
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (res > (2147483647 - (str[i] - '0')) / 10 && sign == 1)
			return (2147483647);
		else if (res > (2147483647 - (str[i] - '0')) / 10 && sign == -1)
			return (-2147483648);
		res = res * 10 + (str[i] - '0');
		i++;
	}
	return (res * sign);
}

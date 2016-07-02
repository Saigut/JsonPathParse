#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREFIX_STREQ(__str, __pre) \
	(strlen(__str) < strlen(__pre) ? 0 : (0 == strncmp(__str, __pre, strlen(__pre))))

int GetToken(char * in_path, char * *out_subpath, char * out_token, int token_buf_len);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return -1;
	}

    char * jsonPath = argv[1];
    char * subpath = jsonPath;

    char token_buf[128];

    int ret = 0;

	while (ret == 0)
	{
		ret = GetToken(subpath, &subpath, token_buf, sizeof(token_buf));
		printf("ret: %d\ntoken: %s\nsubpath: %s\n\n", ret, token_buf, (NULL == subpath)?"NULL":subpath);

		if (strlen(token_buf) <= 0)
		{
			break;
		}

		sleep(1);
	}

    return 0;
}

int GetToken(char * in_path, char * *out_subpath, char * out_token, int token_buf_len)
{
	if (NULL == in_path)
	{
		return -1;
	}

	if (NULL == out_subpath)
	{
		return -1;
	}

	if (NULL == out_token)
	{
		return -1;
	}

	if (token_buf_len < 0)
	{
		return -1;
	}

	memset(out_token, 0, token_buf_len);

	char * path = in_path;

	int parenth_pair = 0;
	int bracket_pair = 0;

	// token. 0, not start. 1, started. 2, ended.
	int token_status = 0;

	char * cur_token = out_token;
	int cur_len = token_buf_len;

	if ('$' == *path)
	{
		if (snprintf(out_token, token_buf_len, "%c", '$') < 0)
		{
			return -1;
		}

		*out_subpath = path + 1;
		return 0;
	}

	if (' ' == *path)
	{
		if (snprintf(out_token, token_buf_len, "%c", '\0') < 0)
		{
			return -1;
		}
		*out_subpath = NULL;
		return 0;
	}

	while ('\0' != *path)
	{
		if (cur_len < 0)
		{
			return -1;
		}

		if ('$' == *path || ' ' == *path)
		{
			if (0 == parenth_pair && 0 == bracket_pair)
			{
				return -1;
			}
			else
			{
				if (snprintf(cur_token, cur_len, "%c", *path) < 0)
				{
					return -1;
				}

				cur_token++;
				cur_len--;
			}
		}
		else if (PREFIX_STREQ(path, ".."))
		{
			if (0 == parenth_pair && 0 == bracket_pair)
			{
				if (0 == token_status)
				{
					token_status = 1;
					if (snprintf(cur_token, cur_len, "%s", "..") < 0)
					{
						return -1;
					}

					cur_token += 2;
					cur_len -= 2;
				}
				else if (1 == token_status)
				{
					token_status = 2;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				if (snprintf(cur_token, cur_len, "%s", "..") < 0)
				{
					return -1;
				}

				cur_token += 2;
				cur_len -= 2;
			}

			if (2 == token_status)
			{
				*out_subpath = path;
				break;
			}

			path+=2;
			continue;
		}
		else if ('.' == *path)
		{
			if (0 == parenth_pair && 0 == bracket_pair)
			{
				if (0 == token_status)
				{
					token_status = 1;
					if (snprintf(cur_token, cur_len, "%s", ".") < 0)
					{
						return -1;
					}

					cur_token++;
					cur_len--;
				}
				else if (1 == token_status)
				{
					token_status = 2;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				if (snprintf(cur_token, cur_len, "%s", ".") < 0)
				{
					return -1;
				}

				cur_token++;
				cur_len--;
			}
		}
		else if ( '[' == *path)
		{
			bracket_pair++;
			if (snprintf(cur_token, cur_len, "%s", "[") < 0)
			{
				return -1;
			}

			cur_token++;
			cur_len--;
		}
		else if (']' == *path)
		{
			if (0 == bracket_pair)
			{
				return -1;
			}

			bracket_pair--;
			if (snprintf(cur_token, cur_len, "%s", "]") < 0)
			{
				return -1;
			}

			cur_token++;
			cur_len--;
		}
		else if ('(' == *path)
		{
			parenth_pair++;
			if (snprintf(cur_token, cur_len, "%s", "(") < 0)
			{
				return -1;
			}

			cur_token++;
			cur_len--;
		}
		else if (')' == *path)
		{
			if (0 == parenth_pair)
			{
				return -1;
			}

			parenth_pair--;
			if (snprintf(cur_token, cur_len, "%s", ")") < 0)
			{
				return -1;
			}

			cur_token++;
			cur_len--;
		}
		else
		{
			if (1 != token_status)
			{
				return -1;
			}

			if (snprintf(cur_token, cur_len, "%c", *path) < 0)
			{
				return -1;
			}

			cur_token++;
			cur_len--;
		}

		if (2 == token_status)
		{
			break;
		}

		path++;
	}
	*out_subpath = path;

	return 0;
}

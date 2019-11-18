#z827

Text compression algorithm designed to take advantage of ASCII characters only actually using 7 bits of a char (the most significant bit is always 0 in ASCII).

Since ASCII characters only actually use 7 bits of an 8 bit char, so I chop off the 8th bit by using bitwise or on the char and an integer (or long, doesn't really matter) initialized to 0. Keep churning out integers this way until the input has been devoured. The process can be reversed by using bitwise or on the resulting integers to create new chars.

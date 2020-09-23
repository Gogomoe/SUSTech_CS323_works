module Unicode where

{-
module Unicode (
    
    ) where

import ...
-}

intToChar :: Int -> Char
intToChar k = toEnum(k)

charToString :: Char -> String
charToString k = (k:"")

intToStr :: Int -> String
intToStr k = (toEnum(k):"")

{- UTF8 bit patterns:
U-00000000 - U-0000007F: 	0xxxxxxx
U-00000080 - U-000007FF: 	110xxxxx 10xxxxxx
U-00000800 - U-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
U-00010000 - U-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
U-00200000 - U-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
U-04000000 - U-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

The code positions U+D800 to U+DFFF (UTF-16 surrogates) 
as well as U+FFFE and U+FFFF must not occur in UTF-8. 

A 1010
B 1011
C 1100
D 1101
E 1110
F 1111
-}

intToUTF8 :: Int -> String
intToUTF8 k = intToUTF8B 0 k

intToUTF8B :: Int -> Int -> String
intToUTF8B b k | k < 0     = error "intToUTF8: negative int"
intToUTF8B b k | (b == 0 || b == 1) && k <= 0x0000007F
    = intToStr k
intToUTF8B b k | (b == 0 || b == 2) && k <= 0x000007FF =
    intToStr(0xC0 + k1) ++ intToStr(0x80 + k0)
    where (k1, k0) = k `quotRem` 0x40
{-
intToUTF8B b k | (b == 0 || b == 2) && k >= 0xD800 && k <= 0xDFFF =
    error "intToUTF8: illegal surrogate number (U+D800 to U+DFFF)"
intToUTF8B b k | (b == 0 || b == 2) && k >= 0xFFFE && k <= 0xFFFF =
    error "intToUTF8: illegal Unicode number (U+FFFE to U+FFFF)"
-}
intToUTF8B b k | (b == 0 || b == 3) && k <= 0x0000FFFF =
    intToStr(0xE0 + k2) ++ intToStr(0x80 + k1) ++ intToStr(0x80 + k0)
    where { (l, k0) = k `quotRem` 0x40;
    (k2, k1) = l `quotRem` 0x40 }
intToUTF8B b k | (b == 0 || b == 4) && k <= 0x001FFFFF =
    intToStr(0xF0 + k3) ++ intToStr(0x80 + k2)
      ++ intToStr(0x80 + k1) ++ intToStr(0x80 + k0)
    where { (l0, k0) = k `quotRem` 0x40;
    (l1, k1) = l0 `quotRem` 0x40;
    (k3, k2) = l1 `quotRem` 0x40 }
intToUTF8B b k | (b == 0 || b == 5) && k <= 0x03FFFFFF =
    intToStr(0xF8 + k4) ++ intToStr(0x80 + k3) ++ intToStr(0x80 + k2)
      ++ intToStr(0x80 + k1) ++ intToStr(0x80 + k0)
    where { (l0, k0) = k `quotRem` 0x40;
    (l1, k1) = l0 `quotRem` 0x40;
    (l2, k2) = l1 `quotRem` 0x40;
    (k4, k3) = l2 `quotRem` 0x40 }
intToUTF8B b k | (b == 0 || b == 6) && k <= 0x7FFFFFFF =
    intToStr(0xFC + k5) ++ intToStr(0x80 + k4) ++ intToStr(0x80 + k3)
      ++ intToStr(0x80 + k2) ++ intToStr(0x80 + k1) ++ intToStr(0x80 + k0)
    where { (l0, k0) = k `quotRem` 0x40;
    (l1, k1) = l0 `quotRem` 0x40;
    (l2, k2) = l1 `quotRem` 0x40;
    (l3, k3) = l2 `quotRem` 0x40;
    (k5, k4) = l3 `quotRem` 0x40 }
intToUTF8B _ _ = error "intToUTF8: overflow"

i5 k = (k4, k3, k2, k1, k0)
    where { (l0, k0) = k `quotRem` 0x40;
    (l1, k1) = l0 `quotRem` 0x40;
    (l2, k2) = l1 `quotRem` 0x40;
    (k4, k3) = l2 `quotRem` 0x40 }



uTF8ToInt :: String -> (Int, String)
uTF8ToInt "" = error "uTF8ToInt: empty string"
uTF8ToInt (x0:xs)    | k0 <= 0x7F =
    (k0, xs)  where k0 = fromEnum(x0)
uTF8ToInt (x0:x1:xs) | k0 >= 0xC0 && k0 <= 0xDF =
    ((k0 - 0xC0)*64 + (k1 - 0x80), xs)
       where { k0 = fromEnum(x0);
         k1 = fromEnum(x1) }
uTF8ToInt (x0:x1:x2:xs) | k0 >= 0xE0 && k0 <= 0xEF =
    ((k0 - 0xE0)*4096 + (k1 - 0x80)*64 + (k2 - 0x80), xs)
       where { k0 = fromEnum(x0);
         k1 = fromEnum(x1);
         k2 = fromEnum(x2) }
uTF8ToInt (x0:x1:x2:x3:xs) | k0 >= 0xF0 && k0 <= 0xF7 =
    ((k0 - 0xF0)*262144 + (k1 - 0x80)*4096 + (k2 - 0x80)*64 + (k3 - 0x80), xs)
       where { k0 = fromEnum(x0);
         k1 = fromEnum(x1);
         k2 = fromEnum(x2);
         k3 = fromEnum(x3) }
uTF8ToInt (x0:x1:x2:x3:x4:xs) | k0 >= 0xF8 && k0 <= 0xFB =
    ((k0 - 0xF8)*16777216 + (k1 - 0x80)*262144 + (k2 - 0x80)*4096
      + (k3 - 0x80)*64 + (k4 - 0x80), xs)
       where { k0 = fromEnum(x0);
         k1 = fromEnum(x1);
         k2 = fromEnum(x2);
         k3 = fromEnum(x3);
         k4 = fromEnum(x4) }
uTF8ToInt (x0:x1:x2:x3:x4:x5:xs) | k0 >= 0xFC && k0 <= 0xFD =
    ((k0 - 0xFC)*1073741824 + (k1 - 0x80)*16777216 + (k2 - 0x80)*262144
      + (k3 - 0x80)*4096 + (k4 - 0x80)*64 + (k5 - 0x80), xs)
       where { k0 = fromEnum(x0);
         k1 = fromEnum(x1);
         k2 = fromEnum(x2);
         k3 = fromEnum(x3);
         k4 = fromEnum(x4);
         k5 = fromEnum(x5) }
uTF8ToInt (x:xs) = error ("uTF8ToInt: not a UTF-8 " ++ show (fromEnum x))

-- Debugging

t1 :: Int -> (Int, String)
t1 = uTF8ToInt . intToUTF8

t2 :: String -> (String, String)
t2 x = (intToUTF8 a, b)  where (a, b) = uTF8ToInt x

iu = intToUTF8
ui = uTF8ToInt


{- UTF8 bit patterns as hexadecimal ranges:

U-00000000 - U-0000007F:
  0xxxxxxx
  0x00-0x7F
U-00000080 - U-000007FF:
  110xxxxx   10xxxxxx
  0xC0-0xDF  0x80-0xBF
  0xC2-0xDF
U-00000800 - U-0000FFFF:
  1110xxxx   10xxxxxx   10xxxxxx
  0xE0-0xEF  0x80-0xBF  0x80-0xBF
  (0xE8, 0xA0)-(0xEF, 0xBF)  0x80-0xBF
U-00010000 - U-001FFFFF:
  11110xxx   10xxxxxx   10xxxxxx   10xxxxxx
  0xF0-0xF7  0x80-0xBF  0x80-0xBF  0x80-0xBF
 (0xF0, 0x90)-(0xF7, 0xBF)
U-00200000 - U-03FFFFFF:
  111110xx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
  0xF8-0xFB  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
 (0xF8, 0x88)-(0xFB, 0xBF)
U-04000000 - U-7FFFFFFF:
  1111110x   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
  0xFC-0xFD  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
  (0xFC, 0x84)-(0xF1, 0xBF)
The code positions U+D800 to U+DFFF (UTF-16 surrogates) 
as well as U+FFFE and U+FFFF must not occur in UTF-8. 

A 1010
B 1011
C 1100
D 1101
E 1110
F 1111
-}

infixl 7  :&
infixl 6  :|

data Range = Range Char Char

instance Show Range where
  show (Range x y) | x == y = showHexChar x
  show (Range x y) = "[" ++ showHexChar x ++ "-" ++ showHexChar y ++ "]"

data Reg   = Reg(Range) | Reg :| Reg | Reg :& Reg

instance Show Reg where
  show (Reg x)  = show x
  show (x :| y) = show x ++ "|" ++ show y
  show (x :& y) = f x y where
    f (a :| b) (c :| d) = "(" ++ show (a :| b) ++ ")(" ++ show (c :| d) ++ ")"
    f (a :| b) c        = "(" ++ show (a :| b) ++ ")" ++ show c
    f  a       (c :| d) = show a ++ "(" ++ show (c :| d) ++ ")"
    f x y               = show x ++ show y


reg     :: Char -> Char -> Reg
reg x y = Reg(Range x y)

reverseReg :: Reg -> Reg
reverseReg (Reg x)  = (Reg x)
reverseReg (x :| y) = (x :| y)
reverseReg (x :& y) = (y :& x)


-- Unicode range to UTF-8 Regular expression
urToRegU8 :: Int -> Int -> Reg
urToRegU8 x y | x < 0 || y < 0 =
    error "urToRegU8: negative argument"
urToRegU8 x y | x > y = error "urToRegU8: not a Unicode range"
{-
urToRegU8 x y | (x >= 0xD800 && x <= 0xDFFF) || (y >= 0xD800 && y <= 0xDFFF) =
    error "urToRegU8: illegal surrogate number (U+D800 to U+DFFF)"
urToRegU8 x y | (x >= 0xFFFE && x <= 0xFFFF) || (y >= 0xFFFE && y <= 0xFFFF) =
    error "urToRegU8: illegal Unicode number (U+FFFE to U+FFFF)"
-}
urToRegU8 x y | (x >= 0x7FFFFFFF || y >= 0x7FFFFFFF) =
    error "urToRegU8: int > 0x7FFFFFFF"
urToRegU8 x y = foldr1 (:|) rs
    where rs = drop ((mlength x)-1) (take (mlength y)
           [ur1 x y, ur2 x y, ur3 x y, ur4 x y, ur5 x y, ur6 x y])

mlength :: Int -> Int
mlength x | x < 0 =
    error "mlength: negative argument"
{-
mlength x | (x >= 0xD800 && x <= 0xDFFF)=
    error "mlength: illegal surrogate number (U+D800 to U+DFFF)"
mlength x | (x >= 0xFFFE && x <= 0xFFFF) =
    error "mlength: illegal Unicode number (U+FFFE to U+FFFF)"
-}
mlength x | x >= 0x7FFFFFFF =
    error "mlength: int > 0x7FFFFFFF"
mlength x | x <= 0x0000007F = 1
mlength x | x <= 0x000007FF = 2
mlength x | x <= 0x0000FFFF = 3
mlength x | x <= 0x001FFFFF = 4
mlength x | x <= 0x03FFFFFF = 5
mlength x | x <= 0x7FFFFFFF = 6

urrange  :: Int -> Int -> (Int, Int)
urrange x y = (mlength x, mlength y)

urA :: Int -> String -> Reg
urA k   _    | k <= 0 = error "urA: first argument non-positive"
urA k (x:xs) | k == 1 = reg x x
urA k (x:xs) = reg x x :& urA (k-1) xs

ur :: Int -> String -> String -> String -> String -> Reg
ur k _ _ _ _ | k <= 0 = error "ur: first argument non-positive" 
ur k xs ys mns mxs = ur0 k (take k xs) (take k ys) (take k mns) (take k mxs)

ur0 :: Int -> String -> String -> String -> String -> Reg
ur0 k (x:xs) (y:ys) _ _ | k == 1 = reg x y
ur0 k (x:xs) (y:ys) (mn:mns) (mx:mxs) | x == y =
     reg x y :& ur0 (k-1) xs ys mns mxs
ur0 k xs ys mns mxs =
   ur01 k (init xs) (last xs) (init ys) (last ys) (init mns) (last mns) (init mxs) (last mxs)

ur01 :: Int -> String -> Char -> String -> Char -> String -> Char -> String -> Char -> Reg
ur01 k xs x ys y mns mn mxs mx | xs == ys
    = ur0 (k-1) xs ys mns mxs :& reg x y
ur01 k xs x ys y mns mn mxs mx | x /= mn
    = urA (k-1) xs :& reg x mx
      :| ur01 k (usucc xs mns mxs) mn ys y mns mn mxs mx
ur01 k xs x ys y mns mn mxs mx | y /= mx
    = urA (k-1) ys :& reg mn y
      :| ur01 k xs x (upred ys mns mxs) mx mns mn mxs mx
ur01 k xs x ys y mns mn mxs mx
    = ur0 (k-1) xs ys mns mxs :& reg mn mx


usucc :: String -> String -> String -> String
usucc xs mns mxs = usucc0 (init xs) (last xs) (init mns) (last mns) (init mxs) (last mxs)

usucc0 :: String -> Char -> String -> Char -> String -> Char -> String
usucc0 xs x mns mn mxs mx | x /= mx = xs ++ [succ x]
usucc0 xs x mns mn mxs mx = usucc xs mns mxs ++ [mn]

upred :: String -> String -> String -> String
upred xs mns mxs = upred0 (init xs) (last xs) (init mns) (last mns) (init mxs) (last mxs)

upred0 :: String -> Char -> String -> Char -> String -> Char -> String
upred0 xs x mns mn mxs mx | x /= mn = xs ++ [pred x]
upred0 xs x mns mn mxs mx = upred xs mns mxs ++ [mx]


-- U-00000000 - U-0000007F:
--  0xxxxxxx
--  0x00-0x7F
ur1 :: Int -> Int -> Reg
ur1 x y = reg x0 y0 
    where { x0 = toEnum (max 0 x); y0 = toEnum (min 0x7f y) }

-- U-00000080 - U-000007FF:
--   110xxxxx   10xxxxxx
--   0xC0-0xDF  0x80-0xBF
ur2 :: Int -> Int -> Reg
ur2 x y = ur 2 sx sy ('\xC0':['\x80']) ('\xDF':['\xBF'])
    where { x0 = max 0x00000080 x; y0 = min 0x000007FF y;
      sx = intToUTF8 x0; sy = intToUTF8 y0 }

-- U-00000800 - U-0000FFFF:
--   1110xxxx   10xxxxxx   10xxxxxx
--   0xE0-0xEF  0x80-0xBF  0x80-0xBF
ur3 :: Int -> Int -> Reg
ur3 x y =
    ur 3 sx sy ('\xE0':repeat '\x80') ('\xEF':repeat '\xBF')
        where { x0 = max 0x00000800 x; y0 = min 0x0000FFFF y;
          sx = intToUTF8 x0; sy = intToUTF8 y0 }

-- U-00010000 - U-001FFFFF:
--  11110xxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xF0-0xF7  0x80-0xBF  0x80-0xBF  0x80-0xBF
ur4 :: Int -> Int -> Reg
ur4 x y = 
    ur 4 sx sy ('\xF0':repeat '\x80') ('\xF7':repeat '\xBF')
        where { x0 = max 0x00010000 x; y0 = min 0x001FFFFF y;
          sx = intToUTF8 x0; sy = intToUTF8 y0 }

-- U-00200000 - U-03FFFFFF:
--  111110xx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xF8-0xFB  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
ur5 :: Int -> Int -> Reg
ur5 x y =
    ur 5 sx sy ('\xF8':repeat '\x80') ('\xFB':repeat '\xBF')
        where { x0 = max 0x00200000 x; y0 = min 0x03FFFFFF y;
          sx = intToUTF8 x0; sy = intToUTF8 y0 }

-- U-04000000 - U-7FFFFFFF:
--  1111110x   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xFC-0xFD  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
ur6 :: Int -> Int -> Reg
ur6 x y =
    ur 5 sx sy ('\xFC':repeat '\x80') ('\xFD':repeat '\xBF')
        where { x0 = max 0x04000000 x; y0 = min 0x7FFFFFFF y;
          sx = intToUTF8 x0; sy = intToUTF8 y0 }


showHex :: Int -> String
showHex k | k < 0  = error "showHex: negative argument"
showHex k | k <= 9 = show k
showHex k | k == 10 = "A"
showHex k | k == 11 = "B"
showHex k | k == 12 = "C"
showHex k | k == 13 = "D"
showHex k | k == 14 = "E"
showHex k | k == 15 = "F"
showHex k = showHex m ++ showHex n   where (m, n) = k `quotRem` 16

showHexChar :: Char -> String
showHexChar '\0'           = "\\0"
showHexChar '\a'           = "\\a"
showHexChar '\b'           = "\\b"
showHexChar '\f'           = "\\f"
showHexChar '\n'           = "\\n"
showHexChar '\r'           = "\\r"
showHexChar '\t'           = "\\t"
showHexChar '\v'           = "\\v"
showHexChar '\\'           = "\\\\"
showHexChar '\''           = "\\\'"
showHexChar '\"'           = "\\\""
showHexChar c | c <= '\xF' = "\\x0" ++ showHex (fromEnum c)
showHexChar c | c >= ' ' && c < '\DEL' = showChar c ""
showHexChar c              = "\\x" ++ showHex (fromEnum c)


-- Unicode to (bigendian) UTF-32 regular expressions:
urToRegU32 :: Int -> Int -> Reg
urToRegU32 x y = 
    ur 4 sx sy ('\x00':repeat '\x00') ('\x7F':repeat '\xFF')
        where { sx = intToUTF32 x; sy = intToUTF32 y }

-- Unicode to (bigendian) UTF-32:
intToUTF32 :: Int -> String
intToUTF32 k | k < 0     = error "intToUTF32: negative int"
intToUTF32 k | k <= 0x7FFFFFFF =
    intToStr(k3) ++ intToStr(k2)
      ++ intToStr(k1) ++ intToStr(k0)
    where { (l0, k0) = k `quotRem` 0x100;
    (l1, k1) = l0 `quotRem` 0x100;
    (k3, k2) = l1 `quotRem` 0x100 }
intToUTF32 _ = error "intToUTF32: int > 0x7FFFFFFF"

-- Unicode to lowendian UTF-32 regular expressions:
urToRegU32L :: Int -> Int -> Reg
urToRegU32L x y = reverseReg(urToRegU32 x y)


-- U-00000000 - U-0000007F:
--  0xxxxxxx
--  0x00-0x7F
urToRegU8B1 :: Int -> Int -> Reg
urToRegU8B1 x y | x < 0 || y < 0 =
    error "urToRegU8B1: negative argument"
urToRegU8B1 x y | x > y = error "urToRegU8B1: not a Unicode range"
urToRegU8B1 x y | x > 0x7F || y > 0x7F = error "urToRegU8B1: argument not a UTF-8 1-byte"
urToRegU8B1 x y = reg (toEnum x) (toEnum y)

-- U-00000080 - U-000007FF:
--   110xxxxx   10xxxxxx
--   0xC0-0xDF  0x80-0xBF
urToRegU8B2 :: Int -> Int -> Reg
urToRegU8B2 x y | x < 0 || y < 0 =
    error "urToRegU8B2: negative argument"
urToRegU8B2 x y | x > y = error "urToRegU8B2: not a Unicode range"
urToRegU8B2 x y | x > 0x000007FF || y > 0x000007FF
    = error "urToRegU8B2: argument not a UTF-8 2-byte"
urToRegU8B2 x y = ur 2 sx sy ('\xC0':['\x80']) ('\xDF':['\xBF'])
    where { sx = intToUTF8B 3 x; sy = intToUTF8B 3 y }

-- U-00000800 - U-0000FFFF:
--   1110xxxx   10xxxxxx   10xxxxxx
--   0xE0-0xEF  0x80-0xBF  0x80-0xBF
urToRegU8B3 :: Int -> Int -> Reg
urToRegU8B3 x y | x < 0 || y < 0 =
    error "urToRegU8B3: negative argument"
urToRegU8B3 x y | x > y = error "urToRegU8B3: not a Unicode range"
urToRegU8B3 x y | x > 0x0000FFFF || y > 0x0000FFFF
    = error "urToRegU8B3: argument not a UTF-8 3-byte"
urToRegU8B3 x y =
    ur 3 sx sy ('\xE0':repeat '\x80') ('\xEF':repeat '\xBF')
        where { sx = intToUTF8B 3 x; sy = intToUTF8B 3 y }

-- U-00010000 - U-001FFFFF:
--  11110xxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xF0-0xF7  0x80-0xBF  0x80-0xBF  0x80-0xBF
urToRegU8B4 :: Int -> Int -> Reg
urToRegU8B4 x y | x < 0 || y < 0 =
    error "urToRegU8B4: negative argument"
urToRegU8B4 x y | x > y = error "urToRegU8B4: not a Unicode range"
urToRegU8B4 x y | x > 0x001FFFFF || y > 0x001FFFFF
    = error "urToRegU8B4: argument not a UTF-8 4-byte"
urToRegU8B4 x y = 
    ur 4 sx sy ('\xF0':repeat '\x80') ('\xF7':repeat '\xBF')
        where { sx = intToUTF8B 4 x; sy = intToUTF8B 4 y }

-- U-00200000 - U-03FFFFFF:
--  111110xx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xF8-0xFB  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
urToRegU8B5 :: Int -> Int -> Reg
urToRegU8B5 x y | x < 0 || y < 0 =
    error "urToRegU8B5: negative argument"
urToRegU8B5 x y | x > y = error "urToRegU8B5: not a Unicode range"
urToRegU8B5 x y | x > 0x03FFFFFF || y > 0x03FFFFFF
    = error "urToRegU8B5: argument not a UTF-8 5-byte"
urToRegU8B5 x y =
    ur 5 sx sy ('\xF8':repeat '\x80') ('\xFB':repeat '\xBF')
        where { sx = intToUTF8B 5 x; sy = intToUTF8B 5 y}

-- U-04000000 - U-7FFFFFFF:
--  1111110x   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx   10xxxxxx
--  0xFC-0xFD  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF  0x80-0xBF
urToRegU8B6 :: Int -> Int -> Reg
urToRegU8B6 x y | x < 0 || y < 0 =
    error "urToRegU8B6: negative argument"
urToRegU8B6 x y | x > y = error "urToRegU8B6: not a Unicode range"
urToRegU8B6 x y | x > 0x7FFFFFFF || y > 0x7FFFFFFF
    = error "urToRegU8B6: argument not a UTF-8 6-byte"
urToRegU8B6 x y =
    ur 6 sx sy ('\xFC':repeat '\x80') ('\xFD':repeat '\xBF')
        where { sx = intToUTF8B 6 x; sy = intToUTF8B 6 y }


from nltk.parse.generate import generate
from nltk import CFG
import random

grammar = CFG.fromstring("""
    CardNum -> 'zero' | Millions | AValue

    Digit -> 'one' | 'two' | 'three' | 'four' | 'five' | 'six' | 'seven' | 'eight' | 'nine'
    Teens ->  'ten' | 'eleven' | 'twelve'  | 'thirteen' | 'fourteen' | 'fifteen' | 'sixteen' | 'seventeen' | 'eighteen' | 'nineteen'
    SecDig -> 'twenty' | 'thirty' | 'forty' | 'fifty' | 'sixty' | 'seventy' | 'eighty' | 'ninety'
    Below100 -> Digit | Teens | SecDig | SecDig '-' Digit

    HundredSfx   -> 'hundred' | 'hundred and ' Below100
    Hundreds     -> Below100 | Digit ' ' HundredSfx

    ThousandSfx  -> 'thousand' | 'thousand ' Hundreds
    Thousands    -> Hundreds | Hundreds ' ' ThousandSfx

    MillionSfx   -> 'million' | 'million ' Thousands
    Millions     -> Thousands | Thousands ' ' MillionSfx

    AValue       -> 'a ' HundredSfx | 'a ' ThousandSfx | 'a hundred ' ThousandSfx | 'a ' MillionSfx | 'a hundred ' MillionSfx
""")

p = 0.3
d = 0.9
for i, sentence in enumerate(generate(grammar)):
    if random.uniform(0,1) < p:
        print(''.join(sentence))
        p = p * d
import sys

TotalCode = ''
LabelTable = {}
LineCount = 0

Value1Reg = "r0"

def UpdateCode(Str):
    global TotalCode
    global LineCount
    TotalCode += (Str +"\n")
    LineCount += 1

def ProcessASM(Line):
    global LabelTable
    global LineCount
    if Line == '':
        return
    try:
        cmd, args = Line.split(" ",1)
        bl = map(lambda x:x.strip(), args.split(","))
    except:
        TotalCode(Line)
        return

    if cmd == ':':
        LabelTable[bl[0]] = LineCount

    elif cmd == ';':
        UpdateCode(Line)
        LineCount -= 1

    elif cmd == 'setStr':
        dstReg = bl[0]
        String = bl[1][1:-1] + "\x00"
        TmpReg = "r255"
        AsciiReg = "r254"

        #Temporary Save the address
        UpdateCode("move %s, %s" % (TmpReg,dstReg ))
        for c in String:
            UpdateCode("puti %s, %d" % (AsciiReg,ord(c)))
            UpdateCode("store %s, %s" % (TmpReg, AsciiReg))
            UpdateCode("add %s, %s" % (TmpReg, Value1Reg))
    elif cmd == 'printf':
        srcReg = bl[0]
        UpdateCode("puts %s" % srcReg)




        

        


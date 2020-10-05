/* 
  ringstepper
  MIT LICENSE Copyright <2020>
  David Reguera Garcia aka Dreg
  Dreg@fr33project.org - http://www.fr33project.org/ - https://github.com/David-Reguera-Garcia-Dreg/
  -
  Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand
  associated documentation files(the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions :
  The above copyright noticeand this permission notice shall be included in all copies or substantial
  portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  -
  WARNING!!bullshit code
  -
*/

#include "ringstepper.h"
#include <stdlib.h>
#include <strsafe.h>

EXT_DECLARE_GLOBALS();

EXT_CLASS::EXT_CLASS()
{
}


#define SSDTOU_SIZE 3000

ULONG ReadSSDT(ULONG64* ssdtou_out)
{
    ULONG ssdt_entries = 0;
    ULONG64 table = 0;
    ULONG64 dir = 0;
    ULONG lim = 0;
    ULONG readb = 0;
    LONG off = 0;
    ULONG platid = 0;
    ULONG major = 0;
    ULONG minor = 0;
    ULONG sp = 0;

    ReadMemory(GetExpression("nt!KiServiceLimit"), &lim, sizeof(lim), &readb);
    if (lim >= SSDTOU_SIZE)
    {
            dprintf("error: nt!KiServiceLimit too long\n");
            return 0;
    }

    ReadPointer(GetExpression("nt!KeServiceDescriptorTable"), &table);

    dir = table;

    g_Ext->m_Control->GetSystemVersion(&platid, &major, &minor, NULL, NULL, NULL, &sp, NULL, NULL, NULL);

#ifdef _WIN64
    for (ssdt_entries = 0; ssdt_entries < lim; ssdt_entries++, dir += sizeof(ULONG))
    {
        ssdtou_out[ssdt_entries] = 0;

        readb = 0;
        ReadMemory(dir, &off, sizeof(off), &readb);
        if (0 == readb)
        {
            return 0;
        }

        if (minor < 6000)
        {
            off &= ~0xF;
        }
        else
        {
            off >>= 4;
        }
        
        ssdtou_out[ssdt_entries] = table + off;
    }
#else
#error  TODO x32 ssdt
#endif 

    return ssdt_entries;
}


void PrintBanner(void)
{
    dprintf("ringstepper " versw " MIT LICENSE Copyright <2020>\n"
        "David Reguera Garcia aka Dreg\n"
        "Dreg@fr33project.org - http://www.fr33project.org/ - https://github.com/David-Reguera-Garcia-Dreg/\n"
        "COMMANDS:\n"
        "!printssdt\n"
        "!z\n"
    );
}


void PrintSSDT(void)
{
    CHAR name[600] = { 0 };
    char* name_addr_to_bp = NULL;
    ULONG name_size = 0;
    ULONG64 disp = 0;
    ULONG64 ssdtou[SSDTOU_SIZE] = { 0 };
    ULONG ssdt_entries = 0;

    PrintBanner();

    ssdt_entries = ReadSSDT(ssdtou);

    while (ssdt_entries > 0)
    {
        memset(name, 0, sizeof(name));
        name_addr_to_bp = "[symbol name not found]";
        name_size = 0;
        disp = 0;
        if (g_Ext->m_Symbols->GetNameByOffset(ssdtou[ssdt_entries - 1], (PSTR)name, _countof(name), &name_size, &disp) == S_OK)
        {
            if (0 == disp && name_size > 0)
            {
                name_addr_to_bp = name;
            }
        }

        dprintf("0x%04X: 0x%016I64X - %s\n",
            ssdt_entries - 1,
            ssdtou[ssdt_entries - 1],
            name_addr_to_bp);
        
        ssdt_entries--;
    }

    return;
}


EXT_COMMAND(printssdt,
    "prints ssdt.\n",
    ""
)
{
    PrintSSDT();
}


ULONG64 Gssdtou[SSDTOU_SIZE] = { 0 };
int Gssdt_entries = 0;


EXT_COMMAND(z,
    "ring step, try it in a syscall instruction.\n",
    ""
)
{
    ULONG64 proc_to_bp = 0;
    ULONG64 thread_to_bp = 0;
    ULONG64 index = 0;
    ULONG64 cip_addr = 0;
    ULONG64 addr_to_bp = 0;
    char* name_addr_to_bp = NULL;
    CHAR name[600] = { 0 };
    ULONG name_size = 0;
    ULONG64 disp = 0;

#ifdef _WIN64
#define INDEX_REG_STR "rax"
#define CIP_REG_STR "rip"
#else
#define INDEX_REG_STR "eax"
#define CIP_REG_STR "eip"
#endif

    if (Gssdt_entries == 0)
    {
        PrintBanner();

        Gssdt_entries = ReadSSDT(Gssdtou);
    }

    proc_to_bp = GetExpression("$proc");
    thread_to_bp = GetExpression("$thread");;

    index = GetExpression(INDEX_REG_STR);
    cip_addr = GetExpression(CIP_REG_STR);
 
    addr_to_bp = Gssdtou[index];
    name_addr_to_bp = "[symbol name not found]";
    if (g_Ext->m_Symbols->GetNameByOffset(Gssdtou[index], (PSTR)name, _countof(name), &name_size, &disp) == S_OK)
    {
        if (0 == disp && name_size > 0)
        {
            name_addr_to_bp = name;
        }
    }

    dprintf(
        "proc_to_bp: 0x%016I64X\n"
        "thread_to_bp: 0x%016I64X\n"
        "addr_to_bp: %s 0x%016I64X\n"
        "%s: 0x%016I64X\n"
        "%s: 0x%016I64X\n",
        proc_to_bp,
        thread_to_bp,
        name_addr_to_bp,
        addr_to_bp,
        INDEX_REG_STR,
        index,
        CIP_REG_STR,
        cip_addr);

    Execute("bp /1 /p %I64X /t %I64X 0x%I64X", proc_to_bp, thread_to_bp, addr_to_bp);
    Execute("bp /1 /p %I64X /t %I64X 0x%I64X", proc_to_bp, thread_to_bp, cip_addr + 2);
    Execute("p", addr_to_bp);
}
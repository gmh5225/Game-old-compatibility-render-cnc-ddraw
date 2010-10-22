/*
 * Copyright (c) 2010 Toni Spets <toni.spets@iki.fi>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <windows.h>
#include <stdio.h>
#include "palette.h"

/* from main */
HRESULT null();

HRESULT ddraw_palette_GetEntries(void *This, DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
{
    printf("DirectDrawPalette::GetEntries(This=%p, dwFlags=%d, dwBase=%d, dwNumEntries=%d, lpEntries=%p)\n", This, (int)dwFlags, (int)dwBase, (int)dwNumEntries, lpEntries);
    return DD_OK;
}

HRESULT ddraw_palette_SetEntries(void *_This, DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
{
    int i;

    fakeDirectDrawPaletteObject *This = (fakeDirectDrawPaletteObject *)_This;

#if _DEBUG
    printf("DirectDrawPalette::SetEntries(This=%p, dwFlags=%d, dwStartingEntry=%d, dwCount=%d, lpEntries=%p)\n", This, (int)dwFlags, (int)dwStartingEntry, (int)dwCount, lpEntries);
#endif

    for(i=0;i<256;i++)
    {
#if USE_OPENGL
        This->data[i] = (lpEntries[i].peBlue<<16)|(lpEntries[i].peGreen<<8)|lpEntries[i].peRed;
#else
        This->data[i] = (lpEntries[i].peRed<<16)|(lpEntries[i].peGreen<<8)|(lpEntries[i].peBlue);
#endif
    }

    return DD_OK;
}

HRESULT ddraw_palette_QueryInterface(void *This, REFIID riid, void **obj)
{
    printf("DirectDrawPalette::QueryInterface(This=%p, riid=%08X, obj=%p)\n", This, (unsigned int)riid, obj);
    return S_OK;
}

ULONG ddraw_palette_AddRef(void *_This)
{
    fakeDirectDrawPaletteObject *This = (fakeDirectDrawPaletteObject *)_This;

    printf("DirectDrawPalette::AddRef(This=%p)\n", This);

    This->Ref++;

    return This->Ref;
}

ULONG ddraw_palette_Release(void *_This)
{
    fakeDirectDrawPaletteObject *This = (fakeDirectDrawPaletteObject *)_This;

    printf("DirectDrawPalette::Release(This=%p)\n", This);

    This->Ref--;

    if(This->Ref == 0)
    {
        free(This);
        return 0;
    }

    return This->Ref;
}

fakeDirectDrawPalette piface =
{
    /* IUnknown */
    ddraw_palette_QueryInterface,
    ddraw_palette_AddRef,
    ddraw_palette_Release,
    /* IDirectDrawPalette */
    null, // ddraw_palette_GetCaps
    ddraw_palette_GetEntries,
    null, // ddraw_palette_Initialize
    ddraw_palette_SetEntries
};

HRESULT ddraw_CreatePalette(void *_This, DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE FAR * lpDDPalette, IUnknown FAR * unkOuter)
{
    fakeDirectDrawPaletteObject *This = (fakeDirectDrawPaletteObject *)_This;

    printf("DirectDraw::CreatePalette(This=%p, dwFlags=%d, DDColorArray=%p, DDPalette=%p, unkOuter=%p)\n", This, (int)dwFlags, lpDDColorArray, lpDDPalette, unkOuter);

    fakeDirectDrawPaletteObject *Palette = (fakeDirectDrawPaletteObject *)malloc(sizeof(fakeDirectDrawPaletteObject));
    Palette->Functions = &piface;
    printf(" Palette = %p\n", Palette);
    *lpDDPalette = (LPDIRECTDRAWPALETTE)Palette;

    ddraw_palette_SetEntries(Palette, dwFlags, 0, 256, lpDDColorArray);

    Palette->Ref = 0;
    ddraw_palette_AddRef(Palette);

    return DD_OK;
}


// Tab Control


#include "quakedef.h"


// initialize control
void CTab_Init(CTab_t *tab)
{
    memset(tab, 0, sizeof(CTab_t));
	tab->lastViewedPage = -1;
	tab->hoveredPage = -1;
}


// clear control
void CTab_Clear(CTab_t *tab)
{
    // no dynamic data, just clear it
    CTab_Init(tab);
}


// create new control
CTab_t * CTab_New(void)
{
    CTab_t *tab = (CTab_t *) Q_malloc(sizeof(CTab_t));
    CTab_Init(tab);
    return tab;
}


// free control
void CTab_Free(CTab_t *tab)
{
    CTab_Clear(tab);
    free(tab);
}


// add tab
void CTab_AddPage(CTab_t *tab, const char *name, int id, const CTabPage_Handlers_t *handlers)
{
    int i;
    CTabPage_t *page;

    if (tab->nPages >= TAB_MAX_TABS)
    {
        Sys_Error("CTab_AddPage: Maximum page number reached "
            "while adding %s", name);
        return;
    }

    for (i=0; i < tab->nPages; i++)
    {
        if (tab->pages[i].id == id)
            Sys_Error("CTab_AddPage: such id already exist");
        if (!strcmp(tab->pages[i].name, name))
            Sys_Error("CTab_AddPage: such name already exist");
    }

    page = &tab->pages[tab->nPages++];

    // set name
    strncpy(page->name, name, TAB_MAX_NAME_LENGTH+1);
    page->name[TAB_MAX_NAME_LENGTH] = 0;

    // set id
    page->id = id;

    // set handlers
    memcpy(&page->handlers, handlers, sizeof(CTabPage_Handlers_t));
}


// draw control
void CTab_Draw(CTab_t *tab, int x, int y, int w, int h)
{
    char line[1024], *s;
    int l = 0, r = 0;
    int i;

	if (tab->activePage != tab->lastViewedPage && tab->activePage < tab->nPages)
	{
		CTabPage_OnShowType onshowFnc = tab->pages[tab->activePage].handlers.onshow;
		if (onshowFnc != NULL) onshowFnc();
		tab->lastViewedPage = tab->activePage;
	}

	tab->width = w;
	tab->height = h;

    // make one string
    strcpy(line, " ");
    for (i=0; i < tab->nPages; i++)
    {
		if (tab->activePage == i)
        {
            line[strlen(line)-1] = 0;
            l = strlen(line);
			strcat(line, "\x10");
        }

		if (tab->hoveredPage == i)
		{
			l = strlen(line) - 1;
		}

        strcat(line, tab->pages[i].name);
        if (tab->activePage == i || tab->hoveredPage == i)
        {
            s = line + l + 1;
            while (*s)
                *s++ ^= 128;
        }

        if (tab->activePage == i)
        {
            r = strlen(line);
            strcat(line, "\x11");
        }
        else
            strcat(line, " ");

		if (tab->hoveredPage == i)
		{
			r = strlen(line);
		}
    }

    // if it is less than screen width, we're done
    if (strlen(line) <= w/8)
    {
        UI_Print_Center(x, y, w, line, false);
    }
    // else, cut sth off
    else
    {
        char *buf;

        int p = 0;
        int q = strlen(line);
        float m = l + (r - l) / 2.0;

        while (q - p  >  w/8)
        {
            if (m-p > q-m)
                p++;
            else
                q--;
        }

        line[q] = 0;
        buf = line + p;

        UI_Print_Center(x, y, w, buf, false);
    }

    // draw separator
    memset(line, '\x1E', w/8);
    line[w/8] = 0;
    line[w/8-1] = '\x1F';
    line[0] = '\x1D';
	memcpy(line + 2, " \x10shift\x11+\x10tab\x11 ", min((w/8)-3, 15));
	memcpy(line + w/8 - 2 - 7, " \x10tab\x11 ", 7);
    UI_Print_Center(x, y+8, w, line, false);

    // draw page
    if (tab->pages[tab->activePage].handlers.draw != NULL)
        tab->pages[tab->activePage].handlers.draw(x, y+16, w, h-16, tab, &tab->pages[tab->activePage]);
}


// process key
int CTab_Key(CTab_t *tab, int key)
{
    int handled;

	if (tab->hoveredPage >= 0)
	{
		tab->activePage = tab->hoveredPage;
		return true;
	}

    // we first call tabs handlers, because they might override
    // default tab keys for modal dialogs
    // tabs are responsible for not using "our" keys for other
    // purposes
    handled = false;
    if (tab->pages[tab->activePage].handlers.key != NULL)
        handled = tab->pages[tab->activePage].handlers.key(key, tab, &tab->pages[tab->activePage]);

    // then try our handlers
    if (!handled)
    {
            switch (key)
            {
			case K_PGUP:
			case K_MOUSE4:
                tab->activePage--;
                handled = true;
                break;

			case K_PGDN:
			case K_MOUSE5:
                tab->activePage++;
                handled = true;
                break;

			case K_LEFTARROW:
				if (isCtrlDown() || isShiftDown()) {
					tab->activePage--;
					handled = true;
				}
				break;

			case K_RIGHTARROW:
				if (isCtrlDown() || isShiftDown()) {
					tab->activePage++;
					handled = true;
				}
				break;

			case K_TAB:
				if (isShiftDown()) tab->activePage--; else tab->activePage++;
				handled = true;
				break;
            }

            if (handled)
                tab->activePage = (tab->activePage + tab->nPages) % tab->nPages;
    }

    // return handled status
    return handled;
}

static qbool CTab_Navi_Mouse_Move (CTab_t *tab, const mouse_state_t *ms) 
{
	if (!tab->width) return false;
	tab->hoveredPage = tab->nPages * (ms->x / (double) tab->width);
	return true;
}

qbool CTab_Mouse_Move(CTab_t *tab, const mouse_state_t *ms)
{
	if (ms->y <= 16) {
		return CTab_Navi_Mouse_Move(tab, ms);
	} else {
		CTabPage_MouseMoveType mmf;
		mouse_state_t lms;

		lms.x = ms->x;
		lms.y = ms->y - 16;
		lms.x_old = ms->x_old;
		lms.y_old = ms->y_old;
		tab->hoveredPage = -1;

		mmf = tab->pages[tab->activePage].handlers.mousemove;
		if (mmf)
			return mmf(&lms);
	}

	return false;
}

// get current page
CTabPage_t * CTab_GetCurrent(CTab_t *tab)
{
    return &tab->pages[tab->activePage];
}


// get current page id
int CTab_GetCurrentId(CTab_t *tab)
{
    return tab->pages[tab->activePage].id;
}


// get current page name
char * CTab_GetCurrentPage(CTab_t *tab)
{
    return tab->pages[tab->activePage].name;
}


// set current page by ID
void CTab_SetCurrentId(CTab_t *tab, int id)
{
    int i;
    for (i=0; i < tab->nPages; i++)
    {
        if (tab->pages[i].id == id)
        {
            tab->activePage = i;
            return;
        }
    }
    Sys_Error("CTab_SetCurrentId: id not found");
}


// set current page by string
void SetCurrentPage(CTab_t *tab, char *name)
{
    int i;
    for (i=0; i < tab->nPages; i++)
    {
        if (!strcmp(tab->pages[i].name, name))
        {
            tab->activePage = i;
            return;
        }
    }
    Sys_Error("CTab_SetCurrentName: name not found");
}

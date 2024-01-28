#include "InterfaceSystem.h"
#include "../ParticuleEngine/ParticuleEngineInput.h"
#include "EventDataSystem.h"
#include "Sprite.h"

int modulo(int a, int b) {
  const int result = a % b;
  return result >= 0 ? result : result + b;
}

Mask* NewMask(int x, int y, int width, int height){
    Mask *mask = malloc(sizeof(Mask));
    mask->x = x;
    mask->y = y;
    mask->width = width;
    mask->height = height;
    return mask;
}

void GetNewRectMask(PC_Rect *src,PC_Rect *dst,Mask* mask){
    if (mask == NULL)
        return;
    int x = fmax(dst->x, mask->x);
    int y = fmax(dst->y, mask->y);
    int x2 = fmin(dst->x + dst->w, mask->x + mask->width);
    int y2 = fmin(dst->y + dst->h, mask->y + mask->height);
    int w = fmax(0, x2 - x);
    int h = fmax(0, y2 - y);

    int w_src = ((float)w / (float)dst->w) * src->w;
    int h_src = ((float)h / (float)dst->h) * src->h;
    int x_src = ((float)(x - dst->x) / (float)dst->w) * src->w;
    int y_src = ((float)(y - dst->y) / (float)dst->h) * src->h;
    x_src += src->x;
    y_src += src->y;

    src->x = x_src;
    src->y = y_src;
    src->w = w_src;
    src->h = h_src;

    dst->x = x;
    dst->y = y;
    dst->w = w;
    dst->h = h;
}

void DrawBorderImageUI(BorderImageUI *border, PC_Rect *rect, Sprite *texture,Mask* mask)
{
    int width = texture->rect.w;
    int height = texture->rect.h;
    if (border->Left == 0 && border->Right == 0 && border->Top == 0 && border->Bottom == 0)
    {
        PC_Rect dstRect = {rect->x, rect->y, rect->w, rect->h};
        PC_Rect srcRect = {texture->rect.x, texture->rect.y, width, height};
        GetNewRectMask(&srcRect,&dstRect,mask);

        PC_DrawSubTextureRect(texture->texture, &srcRect, &dstRect);
        return;
    }
    int x = rect->x;
    int y = rect->y;
    int w = rect->w;
    int h = rect->h;
    PC_Rect srcRectCornerLeftTop = {texture->rect.x, texture->rect.y, border->Left, border->Top};
    PC_Rect dstRectCornerLeftTop = {x, y, border->Left*border->scale, border->Top*border->scale};
    GetNewRectMask(&srcRectCornerLeftTop,&dstRectCornerLeftTop,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectCornerLeftTop, &dstRectCornerLeftTop);

    PC_Rect srcRectCornerRightTop = {texture->rect.x+width - border->Right, texture->rect.y, border->Right, border->Top};
    PC_Rect dstRectCornerRightTop = {x + w - border->Right*border->scale, y, border->Right*border->scale, border->Top*border->scale};
    GetNewRectMask(&srcRectCornerRightTop,&dstRectCornerRightTop,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectCornerRightTop, &dstRectCornerRightTop);

    PC_Rect srcRectCornerLeftBottom = {texture->rect.x, texture->rect.y+height - border->Bottom, border->Left, border->Bottom};
    PC_Rect dstRectCornerLeftBottom = {x, y + h - border->Bottom*border->scale, border->Left*border->scale, border->Bottom*border->scale};
    GetNewRectMask(&srcRectCornerLeftBottom,&dstRectCornerLeftBottom,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectCornerLeftBottom, &dstRectCornerLeftBottom);

    PC_Rect srcRectCornerRightBottom = {texture->rect.x+width - border->Right, texture->rect.y+height - border->Bottom, border->Right, border->Bottom};
    PC_Rect dstRectCornerRightBottom = {x + w - border->Right*border->scale, y + h - border->Bottom*border->scale, border->Right*border->scale, border->Bottom*border->scale};
    GetNewRectMask(&srcRectCornerRightBottom,&dstRectCornerRightBottom,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectCornerRightBottom, &dstRectCornerRightBottom);

    PC_Rect srcRectLeft = {texture->rect.x, texture->rect.y+border->Top, border->Left, height - border->Top - border->Bottom};
    PC_Rect dstRectLeft = {x, y + border->Top*border->scale, border->Left*border->scale, h - border->Top*border->scale - border->Bottom*border->scale};
    GetNewRectMask(&srcRectLeft,&dstRectLeft,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectLeft, &dstRectLeft);

    PC_Rect srcRectRight = {texture->rect.x+width - border->Right, texture->rect.y+border->Top, border->Right, height - border->Top - border->Bottom};
    PC_Rect dstRectRight = {x + w - border->Right*border->scale, y + border->Top*border->scale, border->Right*border->scale, h - border->Top*border->scale - border->Bottom*border->scale};
    GetNewRectMask(&srcRectRight,&dstRectRight,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectRight, &dstRectRight);

    PC_Rect srcRectTop = {texture->rect.x+border->Left, texture->rect.y, width - border->Left - border->Right, border->Top};
    PC_Rect dstRectTop = {x + border->Left*border->scale, y, w - border->Left*border->scale - border->Right*border->scale, border->Top*border->scale};
    GetNewRectMask(&srcRectTop,&dstRectTop,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectTop, &dstRectTop);

    PC_Rect srcRectBottom = {texture->rect.x+border->Left, texture->rect.y+height - border->Bottom, width - border->Left - border->Right, border->Bottom};
    PC_Rect dstRectBottom = {x + border->Left*border->scale, y + h - border->Bottom*border->scale, w - border->Left*border->scale - border->Right*border->scale, border->Bottom*border->scale};
    GetNewRectMask(&srcRectBottom,&dstRectBottom,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectBottom, &dstRectBottom);

    PC_Rect srcRectCenter = {texture->rect.x+border->Left, texture->rect.y+border->Top, width - border->Left - border->Right, height - border->Top - border->Bottom};
    PC_Rect dstRectCenter = {x + border->Left*border->scale, y + border->Top*border->scale, w - border->Left*border->scale - border->Right*border->scale, h - border->Top*border->scale - border->Bottom*border->scale};
    GetNewRectMask(&srcRectCenter,&dstRectCenter,mask);
    PC_DrawSubTextureRect(texture->texture, &srcRectCenter, &dstRectCenter);
}

void InitBorderImageUI(BorderImageUI *border, Sprite *texture)
{
    border->Left = 0;
    border->Right = 0;
    border->Top = 0;
    border->Bottom = 0;
    border->scale = 1;
}

void ChangeBorderImageUI(BorderImageUI *border, int left, int right, int top, int bottom, int scale)
{
    border->Left = left;
    border->Right = right;
    border->Top = top;
    border->Bottom = bottom;
    border->scale = scale;
}

RectTransform* rectTransform_create(Vector2 Position, Vector2 Size, Vector2 AnchorMin, Vector2 AnchorMax)
{
    RectTransform * rect = malloc(sizeof(RectTransform));
    rect->Position = Position;
    rect->Size = Size;
    rect->AnchorMin = AnchorMin;
    rect->AnchorMax = AnchorMax;
    return rect;
}

Vector2 rectTransform_getPosition(WidgetUI * widget)
{
    RectTransform * rect = widget->rect;
    //SCREEN_INIT_SIZE (Vector2) //taille initiale de l'ecran
    //SCREEN_WIDTH,SCREEN_HEIGHT //taille actuelle de l'ecran
    Vector2 pos;
    pos.x = rect->Position.x + widget->parent->Offset.x;
    pos.y = rect->Position.y + widget->parent->Offset.y;
    return pos;//temporaire
    //calcul de la position en fonction de la taille de l'ecran (initial et actuelle) et des ancres
    pos.x = (rect->Position.x + (rect->Size.x * rect->AnchorMin.x)) * (SCREEN_WIDTH / SCREEN_INIT_SIZE.x);
    pos.y = (rect->Position.y + (rect->Size.y * rect->AnchorMin.y)) * (SCREEN_HEIGHT / SCREEN_INIT_SIZE.y);
    return pos;
}

Vector2 rectTransform_getSize(WidgetUI * widget)
{
    RectTransform * rect = widget->rect;
    //SCREEN_INIT_SIZE (Vector2) //taille initiale de l'ecran
    //SCREEN_WIDTH,SCREEN_HEIGHT //taille actuelle de l'ecran
    Vector2 size;
    return rect->Size;//temporaire
    //calcul de la taille en fonction de la taille de l'ecran (initial et actuelle) et des ancres
    size.x = (rect->Size.x * (rect->AnchorMax.x - rect->AnchorMin.x)) * (SCREEN_WIDTH / SCREEN_INIT_SIZE.x);
    size.y = (rect->Size.y * (rect->AnchorMax.y - rect->AnchorMin.y)) * (SCREEN_HEIGHT / SCREEN_INIT_SIZE.y);
    return size;
}


Interface * interface_create(){
    Interface * inter = malloc(sizeof(Interface));
    inter->widgets = List_new();
    inter->Offset = vector2_create(0,0);
    inter->MouseOverWidget = NULL;
    return inter;
}

void interface_clear(Interface * ui)
{
    List_node * node = ui->widgets->head;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget = node->data;
        //free widget
        widget->freeWidget(widget);
        free(widget->rect);
        free(widget);
        node = node->next;
    }
    List_clear(ui->widgets);
}

void interface_free(Interface * ui){
    interface_clear(ui);
    List_free(ui->widgets);
    free(ui);
}

void interface_moveFront(Interface * ui, char * WidgetName,int layer){
    List_node * node = ui->widgets->head;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget2 = node->data;
        if (strcmp(widget2->name, WidgetName) == 0)
        {
            List_deleteAt(ui->widgets, i);
            List_insertAt(ui->widgets, i + layer,widget2);
            return;
        }
        node = node->next;
    }
}

void interface_moveBack(Interface * ui, char * WidgetName,int layer)
{
    List_node * node = ui->widgets->head;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget2 = node->data;
        if (strcmp(widget2->name, WidgetName) == 0)
        {
            List_deleteAt(ui->widgets, i);
            List_insertAt(ui->widgets, i - layer,widget2);
            return;
        }
        node = node->next;
    }
}

void interface_update_set(Interface * ui,WidgetUI * MouseOverWidget)
{
    ui->MouseOverWidget = MouseOverWidget;
    List_node * node = ui->widgets->tail;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget = node->data;
        if (widget->selfActive)
            widget->updateWidget(widget);
        node = node->prev;
    }
    if (eventDataSystem->useController)
        interface_UpdateSwtichSelection(ui);
}

void interface_update(Interface * ui)
{
    interface_update_set(ui,NULL);
}

void interface_transform(Interface * ui, char * WidgetName, Vector2 Position, Vector2 Size, Vector2 AnchorMin, Vector2 AnchorMax)
{
    WidgetUI * widget = interface_getWidgetUI(ui, WidgetName);
    if (widget != NULL)
    {
        RectTransform * rect = widget->rect;
        rect->Position = Position;
        rect->Size = Size;
        rect->AnchorMin = AnchorMin;
        rect->AnchorMax = AnchorMax;
        if (widget->type==BUTTON)
        {
            Button* button = (Button*)widget->data;
            Vector2 txtSize = PC_GetTextSize(button->text, button->font);
            button->posText = vector2_create((rect->Size.x - txtSize.x) / 2, (rect->Size.y - txtSize.y) / 2);
        }
    }
}

void interface_setActive(Interface * ui, char * WidgetName, bool active)
{
    WidgetUI * widget = interface_getWidgetUI(ui, WidgetName);
    if (widget != NULL)
    {
        widget->selfActive = active;
    }
}

void interface_setOffset(Interface * ui, Vector2 Offset)
{
    ui->Offset = Offset;
}

void interface_draw(Interface * ui)
{
    List_node * node = ui->widgets->head;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget = node->data;
        if (widget->selfActive)
        {
                widget->drawWidget(widget);
        }
        node = node->next;
    }
}

void interface_moveWidget(Interface * ui, char * WidgetName, Vector2 Offset)
{
    WidgetUI * widget = interface_getWidgetUI(ui, WidgetName);
    if (widget != NULL)
    {
        RectTransform * rect = widget->rect;
        rect->Position.x = Offset.x;
        rect->Position.y = Offset.y;
    }
}

void interface_remove(Interface * MainUI, char * WidgetName)
{
    WidgetUI * widget = interface_getWidgetUI(MainUI, WidgetName);
    if (widget != NULL)
    {
        widget->freeWidget(widget);
        free(widget->rect);
        free(widget);
        List_remove(MainUI->widgets, widget);
    }
}

WidgetUI* interface_getWidgetUI(Interface * MainUI, char * WidgetName)
{
    List_node * node = MainUI->widgets->head;
    for (int i = 0; i < MainUI->widgets->size; i++)
    {
        WidgetUI * widget = node->data;
        if (strcmp(widget->name, WidgetName) == 0)
        {
            return widget;
        }
        node = node->next;
    }
    return NULL;
}

void* interface_get(Interface * MainUI, char * WidgetName)
{
    WidgetUI * widget = interface_getWidgetUI(MainUI, WidgetName);
    if (widget != NULL)
    {
        return widget->data;
    }
    return NULL;
}

void updateWidgetButton(WidgetUI * button)
{
    Button * b = button->data;
    Vector2 pos = rectTransform_getPosition(button);
    Vector2 size = rectTransform_getSize(button);
    
    if (button->parent->MouseOverWidget == NULL)
    {
        if (PC_MouseOver(pos.x, pos.y, size.x, size.y))
        {
            button->parent->MouseOverWidget = button;
            if ((IsMouseButtonDown(eventDataSystem->mouseLeftClick) || (eventDataSystem->useController && IsKeyDown(eventDataSystem->submitButton2))) && b->enabled)
            {
                //printf("click\n");
                if (b->onClick != NULL)
                    b->onClick(button, b->data);
            }
        }
    }
}
void drawWidgetButton(WidgetUI * button)
{
    Button * b = button->data;
    Vector2 pos = rectTransform_getPosition(button);
    Vector2 size = rectTransform_getSize(button);
    PC_Rect rect = { pos.x, pos.y, size.x, size.y };
    if (b->texture==NULL)
    {
        if (button->parent->MouseOverWidget == button)
        {
            PC_Color color = PC_ColorCreate(fmax(b->colorBackground.r - 90, 0), 
                                                fmax(b->colorBackground.g - 90, 0), 
                                                fmax(b->colorBackground.b - 90, 0), 
                                                b->colorBackground.a);
            PC_DrawFillRect(pos.x, pos.y, size.x, size.y, color);
            PC_DrawRect(pos.x, pos.y, size.x, size.y, PC_ColorCreate(0, 0, 0, 255));
        }
        else
        {
            PC_DrawFillRect(pos.x, pos.y, size.x, size.y, b->colorBackground);
            PC_DrawRect(pos.x, pos.y, size.x, size.y, PC_WHITE);
        }
    }
    else
    {
        if (b->enabled)
        {
            if (button->parent->MouseOverWidget == button)
            {
                //PC_DrawSubTextureSizeColored(b->texture, pos.x, pos.y, 0,0,
                //GetWidth(b->texture),GetHeight(b->texture),size.x,size.y,PC_ColorCreate(165, 165, 165, 255));
                PC_Color tmp = TextureColorMod;
                TextureColorMod = PC_ColorCreate(165, 165, 165, 255);
                DrawBorderImageUI(&b->border,&rect,b->texture,button->mask);
                TextureColorMod = tmp;
            }
            else
            {
                //PC_DrawSubTextureSize(b->texture, pos.x, pos.y, 0,0,GetWidth(b->texture),GetHeight(b->texture),size.x,size.y);
                DrawBorderImageUI(&b->border,&rect,b->texture,button->mask);
            }
        }
        else
        {
            //PC_DrawSubTextureSizeColored(b->texture, pos.x, pos.y, 0,0,GetWidth(b->texture),GetHeight(b->texture),size.x,size.y,PC_ColorCreate(100, 100, 100, 255));
            PC_Color tmp = TextureColorMod;
            TextureColorMod = PC_ColorCreate(100, 100, 100, 255);
            DrawBorderImageUI(&b->border,&rect,b->texture,button->mask);
            TextureColorMod = tmp;
        }
    }
    if (strlen(b->text)>0)
    {
        PC_DrawText((const unsigned char*)b->text, pos.x + b->posText.x, pos.y + b->posText.y, b->colorText, b->font);
    }
}
void freeWidgetButton(WidgetUI * button)
{
    Button * b = button->data;
    free(b);
}

void* interface_make_button(Interface * MainUI, char* WidgetName, char * text, void (*onClick)(WidgetUI*,void *), void *data)
{
    Button * button = malloc(sizeof(Button));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = button;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetButton;
    widget->drawWidget = drawWidgetButton;
    widget->freeWidget = freeWidgetButton;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = BUTTON;
    widget->mask = NULL;
    List_add(MainUI->widgets, widget);

    button->onClick = onClick;
    button->data = data;
    button->text = text;
    button->colorText = PC_ColorCreate(0, 0, 0, 255);
    button->colorBackground = PC_ColorCreate(255, 255, 255, 255);
    button->font = NULL;
    button->texture = NULL;
    button->posText = vector2_create(0, 0);
    button->enabled = 1;
    InitBorderImageUI(&button->border, NULL);
    return button;
}

void updateWidgetLabel(WidgetUI * label)
{}
void drawWidgetLabel(WidgetUI * label)
{
    Label * l = label->data;
    Vector2 pos = rectTransform_getPosition(label);
    if (strlen(l->text) > 0)
        PC_DrawText(l->text, pos.x, pos.y, l->colorText, l->font);
}
void freeWidgetLabel(WidgetUI * label)
{
    Label * l = label->data;
    free(l);
}

void* interface_make_label(Interface * MainUI, char* WidgetName, char * text, PC_Color color, PC_Font * font)
{
    Label * label = malloc(sizeof(Label));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = label;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetLabel;
    widget->drawWidget = drawWidgetLabel;
    widget->freeWidget = freeWidgetLabel;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = LABEL;
    List_add(MainUI->widgets, widget);

    label->text = text;
    label->colorText = color;
    label->font = font;
    return label;
}

void updateWidgetInputField(WidgetUI * inputField)
{
    InputField * i = inputField->data;
    Vector2 pos = rectTransform_getPosition(inputField);
    Vector2 size = rectTransform_getSize(inputField);
    if (inputField->parent->MouseOverWidget == NULL)
    {
        if (PC_MouseOver(pos.x, pos.y, size.x, size.y))
        {
            inputField->parent->MouseOverWidget = inputField;
            if ((IsMouseButtonDown(eventDataSystem->mouseLeftClick) || (eventDataSystem->useController && IsKeyDown(eventDataSystem->submitButton2))))
            {
                i->isFocused = true;
            }
        }
        else
        {
            if ((IsMouseButtonDown(eventDataSystem->mouseLeftClick) || (eventDataSystem->useController && IsKeyDown(eventDataSystem->submitButton2))))
            {
                i->isFocused = false;
            }
        }
        if (i->isFocused)
        {
            if (IsKeyDown(eventDataSystem->deleteButton))
            {
                size_t len = strlen(i->text);
                if (len > 0)
                {
                    //realloc
                    i->text = (char*)realloc(i->text, len);
                    i->text[len - 1] = '\0';
                }
            }
            else
            {
                char* text = PC_GetText();
                if (text != NULL)
                {
                    size_t len = strlen(i->text);
                    i->text = (char*)realloc(i->text, len + strlen(text) + 1);
                    strcat(i->text, text);
                    
                    Vector2 size2 = PC_GetTextSize((const unsigned char *)i->text, i->font);
                    if (size2.x+5 > size.x)
                    {
                        i->text = (char*)realloc(i->text, len + 1);
                        i->text[len] = '\0';
                    }
                    free(text);
                }
            }
            if (eventDataSystem->useController)
            {
                i->isFocused = false;
            }
        }
    }
}
void drawWidgetInputField(WidgetUI * inputField)
{
    InputField * i = inputField->data;
    Vector2 pos = rectTransform_getPosition(inputField);
    Vector2 size = rectTransform_getSize(inputField);
    if (i->texture == NULL)
    {
        PC_DrawFillRect(pos.x, pos.y, size.x, size.y, PC_BLACK);
        PC_DrawRect(pos.x, pos.y, size.x, size.y, PC_WHITE);
    }
    else
    {
        PC_DrawSubTextureSize(i->texture->texture, pos.x, pos.y, i->texture->rect.x, i->texture->rect.y,i->texture->rect.w, i->texture->rect.h, size.x, size.y);
    }
    if (strlen(i->text) > 0)
    {
        PC_DrawText((const unsigned char*)i->text, pos.x+5, pos.y+5, i->colorText, i->font);
    }
}
void freeWidgetInputField(WidgetUI * inputField)
{
    InputField * i = inputField->data;
    free(i->text);
    free(i);
}

void* interface_make_inputField(Interface * MainUI, char* WidgetName,PC_Color color, PC_Font * font)
{
    InputField * inputField = malloc(sizeof(InputField));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = inputField;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetInputField;
    widget->drawWidget = drawWidgetInputField;
    widget->freeWidget = freeWidgetInputField;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = INPUT_FIELD;
    List_add(MainUI->widgets, widget);

    inputField->text = malloc(sizeof(char) *1);
    inputField->text[0] = '\0';
    inputField->colorText = color;
    inputField->font = font;
    inputField->texture = NULL;
    inputField->isFocused = false;
    return inputField;
}

void updateWidgetImage(WidgetUI * image)
{
    Vector2 pos = rectTransform_getPosition(image);
    Vector2 size = rectTransform_getSize(image);
    if (image->parent->MouseOverWidget == NULL)
    {
        if (PC_MouseOver(pos.x, pos.y, size.x, size.y))
        {
            image->parent->MouseOverWidget = image;
        }
    }
}
void drawWidgetImage(WidgetUI * image)
{
    Image * i = image->data;
    Vector2 pos = rectTransform_getPosition(image);
    Vector2 size = rectTransform_getSize(image);
    if (i->texture!=NULL)
    {
        PC_DrawSubTextureSize(i->texture->texture, pos.x, pos.y, i->texture->rect.x,i->texture->rect.y,i->texture->rect.w,i->texture->rect.h,size.x,size.y);        
    }
}
void freeWidgetImage(WidgetUI * image)
{
    Image * i = image->data;
    free(i);
}

void* interface_make_image(Interface * MainUI, char* WidgetName, Sprite * texture)
{
    Image * image = malloc(sizeof(Image));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = image;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetImage;
    widget->drawWidget = drawWidgetImage;
    widget->freeWidget = freeWidgetImage;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = IMAGE;
    List_add(MainUI->widgets, widget);

    image->texture = texture;
    return image;
}

void updateWidgetScrollBar(WidgetUI * scrollBar)
{
    ScrollBar * s = scrollBar->data;
    Vector2 pos = rectTransform_getPosition(scrollBar);
    Vector2 size = rectTransform_getSize(scrollBar);
    if (scrollBar->parent->MouseOverWidget == NULL)
    {
        if (PC_MouseOver(pos.x, pos.y, size.x, size.y))
        {
            scrollBar->parent->MouseOverWidget = scrollBar;
            if ((IsMouseButtonDown(eventDataSystem->mouseLeftClick) || (eventDataSystem->useController && IsKeyDown(eventDataSystem->submitButton2))))
            {
                s->isFocused = true;
                if (eventDataSystem->useController)
                {
                    //set MousePosition as controller position
                    if (scrollBar->rect->Size.x > scrollBar->rect->Size.y)
                    {
                        //horizontal
                        MousePosition.x = pos.x + (s->value * size.x / s->maxValue);
                    }
                    else
                    {
                        //vertical
                        MousePosition.y = pos.y + (s->value * size.y / s->maxValue);
                    }
                }
            }
        }
        if (eventDataSystem->useController)
        {
            if (!PC_MouseOver(pos.x, pos.y, size.x, size.y) || IsKeyDown(eventDataSystem->cancelButton))
            {
                s->isFocused = false;
            }
        }
        else
        {
            if (!IsMouseButtonPressed(eventDataSystem->mouseLeftClick) && !(IsMouseButtonDown(eventDataSystem->mouseLeftClick) || (eventDataSystem->useController && IsKeyDown(eventDataSystem->submitButton2))))
            {
                s->isFocused = false;
            }
        }
        if (s->isFocused)
        {
            if (scrollBar->rect->Size.x > scrollBar->rect->Size.y)
            {
                //horizontal
                s->value = (int)((MousePosition.x - pos.x) * s->maxValue / size.x);
            }
            else
            {
                //vertical
                s->value = (int)((MousePosition.y - pos.y) * s->maxValue / size.y);
            }
            if (s->value < 0)
            {
                s->value = 0;
            }
            if (s->value > s->maxValue)
            {
                s->value = s->maxValue;
            }
        }        
    }
    if (s->onUpdate != NULL && s->isFocused)
    {
        s->onUpdate(scrollBar, s->data);
    }
}
void drawWidgetScrollBar(WidgetUI * scrollBar)
{
    ScrollBar * s = scrollBar->data;
    Vector2 pos = rectTransform_getPosition(scrollBar);
    Vector2 size = rectTransform_getSize(scrollBar);

    PC_DrawFillRect(pos.x, pos.y, size.x, size.y, PC_BLACK);
    PC_DrawRect(pos.x, pos.y, size.x, size.y, PC_WHITE);
    if (scrollBar->rect->Size.x > scrollBar->rect->Size.y)
    {
        //horizontal
        unsigned int v = (int)(size.x * (s->value / (float)s->maxValue));
        if (v < (s->sizeScroll/2))
        {
            v = (s->sizeScroll/2);
        }
        if (v > size.x-(s->sizeScroll/2))
        {
            v = size.x-(s->sizeScroll/2);
        }
        PC_DrawFillRect(pos.x + v - (s->sizeScroll/2), pos.y, s->sizeScroll, size.y, PC_WHITE);
    }
    else
    {
        //vertical
        int v = (int)(size.y * (s->value / (float)s->maxValue));
        if (v < (s->sizeScroll/2))
        {
            v = (s->sizeScroll/2);
        }
        if (v > size.y - (s->sizeScroll/2))
        {
            v = size.y - (s->sizeScroll/2);
        }
        PC_DrawFillRect(pos.x, pos.y + v - (s->sizeScroll/2), size.x, s->sizeScroll, PC_WHITE);
    }
}
void freeWidgetScrollBar(WidgetUI * scrollBar)
{
    ScrollBar * s = scrollBar->data;
    free(s);
}

void* interface_make_scrollBar(Interface * MainUI, char* WidgetName, int maxValue, void (*onUpdate)(WidgetUI*,void *), void *data)
{
    ScrollBar * scrollBar = malloc(sizeof(ScrollBar));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = scrollBar;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetScrollBar;
    widget->drawWidget = drawWidgetScrollBar;
    widget->freeWidget = freeWidgetScrollBar;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = SCROLL_BAR;
    List_add(MainUI->widgets, widget);

    scrollBar->maxValue = maxValue;
    scrollBar->value = 0;
    scrollBar->onUpdate = onUpdate;
    scrollBar->data = data;
    scrollBar->sizeScroll = 20;
    return scrollBar;
}

void updateWidgetPanel(WidgetUI * panel)
{
    Panel * p = panel->data;
    Vector2 pos = rectTransform_getPosition(panel);
    Vector2 size = rectTransform_getSize(panel);

    p->ui->Offset.x = pos.x;
    p->ui->Offset.y = pos.y;
    interface_update_set(p->ui,panel->parent->MouseOverWidget);
    panel->parent->MouseOverWidget = p->ui->MouseOverWidget;
}
void drawWidgetPanel(WidgetUI * panel)
{
    Panel * p = panel->data;
    Vector2 pos = rectTransform_getPosition(panel);

    p->ui->Offset.x = pos.x;
    p->ui->Offset.y = pos.y;
    interface_draw(p->ui);
}
void freeWidgetPanel(WidgetUI * panel)
{
    Panel * p = panel->data;
    interface_free(p->ui);
    free(p);
}

void* interface_make_panel(Interface * MainUI, char* WidgetName)
{
    Panel * panel = malloc(sizeof(Panel));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = panel;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetPanel;
    widget->drawWidget = drawWidgetPanel;
    widget->freeWidget = freeWidgetPanel;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = PANEL;
    List_add(MainUI->widgets, widget);

    panel->ui = interface_create();
    return panel;
}

void updateWidgetRect(WidgetUI * rect)
{
    Vector2 pos = rectTransform_getPosition(rect);
    Vector2 size = rectTransform_getSize(rect);
    if (rect->parent->MouseOverWidget == NULL)
    {
        if (PC_MouseOver(pos.x, pos.y, size.x, size.y))
        {
            rect->parent->MouseOverWidget = rect;
        }
    }
}
void drawWidgetRect(WidgetUI * rect)
{
    Rect * r = rect->data;
    Vector2 pos = rectTransform_getPosition(rect);
    Vector2 size = rectTransform_getSize(rect);

    PC_DrawFillRect(pos.x, pos.y, size.x, size.y, r->color);
}
void freeWidgetRect(WidgetUI * rect)
{
    Rect * r = rect->data;
    free(r);
}

void* interface_make_rect(Interface * MainUI, char* WidgetName, PC_Color color)
{
    Rect * rect = malloc(sizeof(Rect));
    WidgetUI * widget = malloc(sizeof(WidgetUI));
    widget->data = rect;
    widget->name = WidgetName;
    widget->selfActive = true;
    widget->updateWidget = updateWidgetRect;
    widget->drawWidget = drawWidgetRect;
    widget->freeWidget = freeWidgetRect;
    widget->rect = rectTransform_create(vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0), vector2_create(0, 0));
    widget->parent = MainUI;
    widget->type = RECTANGLE;
    List_add(MainUI->widgets, widget);

    rect->color = color;
    return rect;
}

Vector2 GetCenterWidget(WidgetUI * widget)
{
    Vector2 pos = rectTransform_getPosition(widget);
    Vector2 size = rectTransform_getSize(widget);
    return vector2_create(pos.x + size.x / 2, pos.y + size.y / 2);
}

int Sign(int x)
{
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

WidgetUI* NearWidget(Interface* ui, WidgetUI* exclude, Vector2 direction){
    List_node * node = ui->widgets->head;
    int min = sqrt(SCREEN_HEIGHT * SCREEN_HEIGHT + SCREEN_WIDTH * SCREEN_WIDTH);
    WidgetUI* near = NULL;
    for (int i = 0; i < ui->widgets->size; i++)
    {
        WidgetUI * widget = node->data;
        //check if widget is near
        if (!(widget == exclude || !widget->selfActive 
                    || widget->type == PANEL || widget->type == LABEL || widget->type == IMAGE))
        {
            //check if widget is near of MousePosition
            Vector2 center = GetCenterWidget(widget);
            Vector2 dir = vector2_create(-(MousePosition.x - center.x), -(MousePosition.y - center.y));
            int dist = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (dist < min)
            {
                if (direction.x == 0 && direction.y == 0)
                {
                    min = dist;
                    near = widget;
                }
                else
                {
                    Vector2 dir2;
                    //check if widget is near of direction
                    if (fabs(dir.x) < fabs(dir.y))
                    {
                        dir2 = vector2_create(0, Sign(dir.y)); //sign(dir.y) = 1 or -1
                    }
                    else
                    {
                        dir2 = vector2_create(Sign(dir.x), 0);
                    }
                    if (dir2.x == direction.x && dir2.y == direction.y)
                    {
                        min = dist;
                        near = widget;
                    }                        
                }
            }
        }
        node = node->next;
    }
    return near;
}


void interface_UpdateSwtichSelection(Interface * MainUI)
{
    if (eventDataSystem->selectedUI != NULL && eventDataSystem->selectedUI == MainUI)
    {
        if (eventDataSystem->selectedUI->MouseOverWidget == NULL)
        {
            //get widget near mouse
            WidgetUI * nearWidget = NearWidget(MainUI, NULL, vector2_create(0, 0));
            if (nearWidget != NULL)
            {
                MousePosition = GetCenterWidget(nearWidget);
            }
        }
        else
        {
            WidgetUI * nearWidget = NULL;
            if (eventDataSystem->selectedUI->MouseOverWidget->type == SCROLL_BAR &&
                ((ScrollBar*)eventDataSystem->selectedUI->MouseOverWidget->data)->isFocused)
            {
                WidgetUI * scrollBar = eventDataSystem->selectedUI->MouseOverWidget;
                if (scrollBar->rect->Size.x > scrollBar->rect->Size.y)
                {
                    //horizontal
                    if (IsKeyDown(eventDataSystem->upButton))//get widget near top
                        nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(0, -1));
                    else if (IsKeyDown(eventDataSystem->downButton))//get widget near bottom
                        nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(0, 1));

                    else if (IsKeyPressed(eventDataSystem->leftButton))//get widget near left
                        MousePosition.x -= 1;
                    else if (IsKeyPressed(eventDataSystem->rightButton))//get widget near right
                        MousePosition.x += 1;
                }
                else
                {
                    //vertical
                    if (IsKeyDown(eventDataSystem->leftButton))//get widget near left
                        nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(-1, 0));
                    else if (IsKeyDown(eventDataSystem->rightButton))//get widget near right
                        nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(1, 0));

                    else if (IsKeyPressed(eventDataSystem->upButton))//get widget near top
                        MousePosition.y -= 1;
                    else if (IsKeyPressed(eventDataSystem->downButton))//get widget near bottom
                        MousePosition.y += 1;
                }
            }
            else
            {
                if (IsKeyDown(eventDataSystem->upButton))//get widget near top
                    nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(0, -1));
                else if (IsKeyDown(eventDataSystem->downButton))//get widget near bottom
                    nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(0, 1));
                else if (IsKeyDown(eventDataSystem->leftButton))//get widget near left
                    nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(-1, 0));
                else if (IsKeyDown(eventDataSystem->rightButton))//get widget near right
                    nearWidget = NearWidget(MainUI, MainUI->MouseOverWidget, vector2_create(1, 0));

                if (nearWidget != NULL)
                    MousePosition = GetCenterWidget(nearWidget);
            }
            if (nearWidget != NULL)
                    MousePosition = GetCenterWidget(nearWidget);
        }
        
    }
}

void interface_DisableSwitchSelection()
{
    eventDataSystem->selectedUI = NULL;
    MousePosition.x = -1;
    MousePosition.y = -1;
}
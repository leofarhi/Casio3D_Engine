#ifndef INTERFACESYSTEM_H
#define INTERFACESYSTEM_H
#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/ParticuleEngineDraw.h"
#include "../ParticuleEngine/ParticuleEngineTexture.h"
#include "../ParticuleEngine/ParticuleEngineInput.h"
#include "../ParticuleEngine/ParticuleEngineFont.h"
#include "../ParticuleEngine/Resources.h"
#include <List.h>
#include "EventDataSystem.h"

int modulo(int a, int b);

typedef struct Interface Interface;
typedef struct WidgetUI WidgetUI;
typedef struct Sprite Sprite;

typedef struct Mask{
    int x;
    int y;
    int width;
    int height;
}Mask;

Mask* NewMask(int x, int y, int width, int height);
void GetNewRectMask(PC_Rect *src,PC_Rect *dst,Mask* mask);


typedef struct BorderImageUI{
    int Left;
    int Right;
    int Top;
    int Bottom;
    int scale;
}BorderImageUI;

void DrawBorderImageUI(BorderImageUI *border, PC_Rect *rect, Sprite *texture, Mask *mask);

#define SCREEN_INIT_SIZE (Vector2){SCREEN_WIDTH,SCREEN_HEIGHT} //taille de l'ecran (800x600)

typedef struct RectTransform
{
    Vector2 Position; //PosX, PosY //position initiale du rectangle
    Vector2 Size; //Width, Height //taille initiale du rectangle
    Vector2 AnchorMin; //AnchorMinX, AnchorMinY
    Vector2 AnchorMax; //AnchorMaxX, AnchorMaxY
} RectTransform;

RectTransform* rectTransform_create(Vector2 Position, Vector2 Size, Vector2 AnchorMin, Vector2 AnchorMax);

Vector2 rectTransform_getPosition(WidgetUI * widget);
//rectTransform_getPosition permet de recuperer la position d'un rectangle
//celle ci est calculee en fonction de la taille de l'ecran
//rect est le rectangle dont on veut recuperer la position

Vector2 rectTransform_getSize(WidgetUI * widget);
//rectTransform_getSize permet de recuperer la taille d'un rectangle
//celle ci est calculee en fonction de la taille de l'ecran
//rect est le rectangle dont on veut recuperer la taille

typedef struct Button{
    char *text; //texte
    Sprite *texture;
    PC_Color colorText;
    PC_Color colorBackground;
    PC_Font *font;
    Vector2 posText; //position du texte
    void (*onClick)(WidgetUI*,void *); //fonction a executer lors du click
    void *data; //données a passer a la fonction

    BorderImageUI border;
    int enabled; //booleen si le bouton est actif
}Button;

typedef struct Label{
    char *text; //texte
    PC_Color colorText;
    PC_Font *font;
}Label;

typedef struct InputField{
    char *text; //texte
    Sprite *texture;
    PC_Color colorText;
    PC_Font *font;
    bool isFocused; //booleen si le champ est selectionne
}InputField;

typedef struct Image{
    Sprite *texture;
    BorderImageUI border;
}Image;

typedef struct ScrollBar{
    int value; //valeur
    int maxValue; //valeur max
    unsigned int sizeScroll; //taille du scroll
    bool isFocused; //booleen si le champ est selectionne
    //si le champ est selectionné
    void (*onUpdate)(WidgetUI*,void *); //fonction a executer lors d'un rafraichissement
    void *data; //données a passer a la fonction
}ScrollBar;

typedef struct Panel{
    Interface* ui; //interface
}Panel;

typedef struct Rect{
    PC_Color color; //couleur
}Rect;

typedef enum InterfaceType{ //type d'interface
    BUTTON,
    LABEL,
    INPUT_FIELD,
    IMAGE,
    SCROLL_BAR,
    PANEL,
    RECTANGLE
} InterfaceType;

typedef struct WidgetUI{ //widget de l'interface
    RectTransform* rect; //rectangle de l'interface
    InterfaceType type; //type de l'interface
    Interface *parent; //interface parent
    void *data; //element de l'interface
    char *name; //nom de l'interface
    int selfActive; // booleen update et draw
    void (*freeWidget)(WidgetUI *); //fonction de liberation de l'interface
    void (*drawWidget)(WidgetUI *); //fonction de dessin de l'interface
    void (*updateWidget)(WidgetUI *); //fonction de mise a jour de l'interface

    Mask* mask; //masque de collision //valeur NULL si pas de masque
}WidgetUI;

typedef struct Interface{ //interface graphique
    List * widgets; //liste des widgets de l'interface
    Vector2 Offset; //offset de l'interface, permet de deplacer l'interface
    WidgetUI * MouseOverWidget; //widget survole par la souris
}Interface;

Interface * interface_create();
//interface_create permet de creer une interface
//sa valeur est un pointeur vers l'interface creee

void interface_free(Interface * ui);
//interface_free permet de liberer une interface
//ui est l'interface a liberer

void interface_moveFront(Interface * ui, char * WidgetName,int layer);
//interface_moveFront permet de deplacer un widget vers l'avant
//ui est l'interface dans laquelle on veut deplacer le widget
//widgetName est le nom du widget a deplacer
//layer est le nombre de couches a deplacer le widget

void interface_moveBack(Interface * ui, char * WidgetName,int layer);
//interface_moveBack permet de deplacer un widget vers l'arriere
//ui est l'interface dans laquelle on veut deplacer le widget
//widgetName est le nom du widget a deplacer
//layer est le nombre de couches a deplacer le widget

void ChangeBorderImageUI(BorderImageUI *border, int left, int right, int top, int bottom, int scale);
//ChangeBorderImageUI permet de changer les bordures d'une image
//border est les bordures de l'image
//left est la bordure gauche
//right est la bordure droite
//top est la bordure du haut
//bottom est la bordure du bas
//scale est le facteur de redimensionnement

void interface_transform(Interface * ui, char * WidgetName, Vector2 Position, Vector2 Size, Vector2 AnchorMin, Vector2 AnchorMax);
//interface_transform permet de transformer un widget
//ui est l'interface dans laquelle on veut transformer le widget
//widgetName est le nom du widget a transformer
//Position est la position du widget
//Size est la taille du widget
//AnchorMin est le point d'ancrage min du widget
//AnchorMax est le point d'ancrage max du widget

void interface_setActive(Interface * ui, char * WidgetName, bool active);
//interface_setActive permet de rendre actif ou non un widget
//ui est l'interface dans laquelle on veut rendre actif ou non le widget
//widgetName est le nom du widget a rendre actif ou non
//active est le booleen qui permet de rendre actif ou non le widget

void interface_setOffset(Interface * ui, Vector2 Offset);
//interface_setOffset permet de deplacer l'interface
//ui est l'interface a deplacer
//Offset est le vecteur de deplacement

void interface_draw(Interface * ui);
//interface_draw permet de dessiner l'interface
//ui est l'interface a dessiner

void interface_update(Interface * ui);
//interface_update permet de mettre a jour l'interface
//ui est l'interface a mettre a jour

void interface_update_set(Interface * ui, WidgetUI * MouseOverWidget);
//update permet de mettre a jour l'interface
//ui est l'interface a mettre a jour
//MouseOverWidget permet de definir le widget sur lequel le curseur est (NULL si aucun widget n'est selectionne)

void interface_moveWidget(Interface * ui, char * WidgetName, Vector2 Offset);
//interface_moveWidget permet de deplacer un widget
//ui est l'interface dans laquelle on veut deplacer le widget
//widgetName est le nom du widget a deplacer
//Offset est le vecteur de deplacement

void interface_remove(Interface * MainUI, char * WidgetName);
//interface_remove permet de supprimer un widget
//MainUI est l'interface dans laquelle on veut supprimer le widget
//WidgetName est le nom du widget a supprimer

WidgetUI* interface_getWidgetUI(Interface * MainUI, char * WidgetName);
//interface_getWidgetUI permet de recuperer un widget
//MainUI est l'interface dans laquelle on veut recuperer le widget
//WidgetName est le nom du widget a recuperer

void* interface_get(Interface * MainUI, char * WidgetName);
//interface_get permet de recuperer la data d'un widget
//MainUI est l'interface dans laquelle on veut recuperer le widget
//WidgetName est le nom du widget a recuperer

void interface_clear(Interface * MainUI);
//interface_clear permet de supprimer tous les widgets d'une interface
//MainUI est l'interface dans laquelle on veut supprimer tous les widgets

void* interface_make_button(Interface * MainUI, char* WidgetName, char * text, void (*onClick)(WidgetUI*,void *), void *data);
//interface_make_button permet de creer un bouton
//MainUI est l'interface dans laquelle on veut creer le bouton
//WidgetName est le nom du bouton
//text est le texte du bouton
//onClick est la fonction a executer lors du click
//data est les donnees a passer a la fonction

void* interface_make_label(Interface * MainUI, char* WidgetName, char * text, PC_Color color, PC_Font * font);
//interface_make_label permet de creer un label
//MainUI est l'interface dans laquelle on veut creer le label
//WidgetName est le nom du label
//text est le texte du label
//color est la couleur du label

void* interface_make_inputField(Interface * MainUI, char* WidgetName,PC_Color color, PC_Font * font);
//interface_make_inputField permet de creer un champ de texte
//MainUI est l'interface dans laquelle on veut creer le champ de texte
//WidgetName est le nom du champ de texte

void* interface_make_image(Interface * MainUI, char* WidgetName, Sprite * texture);
//interface_make_image permet de creer une image
//MainUI est l'interface dans laquelle on veut creer l'image
//WidgetName est le nom de l'image
//texture est la texture de l'image

void* interface_make_scrollBar(Interface * MainUI, char* WidgetName, int maxValue, void (*onUpdate)(WidgetUI*,void *), void *data);
//interface_make_scrollBar permet de creer une barre de defilement
//MainUI est l'interface dans laquelle on veut creer la barre de defilement
//WidgetName est le nom de la barre de defilement
//maxValue est la valeur max de la barre de defilement
//onUpdate est la fonction a executer lors d'un rafraichissement
//data est les donnees a passer a la fonction

void* interface_make_panel(Interface * MainUI, char* WidgetName);
//interface_make_panel permet de creer un panel
//MainUI est l'interface dans laquelle on veut creer le panel
//WidgetName est le nom du panel

void* interface_make_rect(Interface * MainUI, char* WidgetName, PC_Color color);
//interface_make_rect permet de creer un rectangle
//MainUI est l'interface dans laquelle on veut creer le rectangle
//WidgetName est le nom du rectangle
//color est la couleur du rectangle

void interface_UpdateSwtichSelection(Interface * MainUI);
//interface_UpdateSwtichSelection permet de changer MouseOverWidget si le support ne permet pas de le faire
//certains supports n'ont pas de souris, donc il faut changer MouseOverWidget a l'aide de la touche directionnelle
//MainUI est l'interface a mettre a jour

void interface_DisableSwitchSelection();
//interface_DisableSwitchSelection permet de desactiver les interactions avec le menu

#endif
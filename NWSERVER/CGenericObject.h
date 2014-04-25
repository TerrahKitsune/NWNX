#ifndef _NX_NWN_STRUCT_CGENERICOBJECT_
#define _NX_NWN_STRUCT_CGENERICOBJECT_

#define CGameObject__OBJECT_TYPE_GUI 0x1
#define CGameObject__OBJECT_TYPE_TILE 0x2
#define CGameObject__OBJECT_TYPE_MODULE 0x3
#define CGameObject__OBJECT_TYPE_AREA 0x4
#define CGameObject__OBJECT_TYPE_CREATURE 0x5
#define CGameObject__OBJECT_TYPE_ITEM 0x6
#define CGameObject__OBJECT_TYPE_TRIGGER 0x7
#define CGameObject__OBJECT_TYPE_PROJECTILE 0x8
#define CGameObject__OBJECT_TYPE_PLACEABLE 0x9
#define CGameObject__OBJECT_TYPE_DOOR 0xA
#define CGameObject__OBJECT_TYPE_AREAOFEFFECTOBJECT 0xB
#define CGameObject__OBJECT_TYPE_WAYPOINT 0xC
#define CGameObject__OBJECT_TYPE_ENCOUNTER 0xD
#define CGameObject__OBJECT_TYPE_STORE 0xE
#define CGameObject__OBJECT_TYPE_PORTAL 0xF
#define CGameObject__OBJECT_TYPE_SOUND 0x10

struct CGenericObject_s {
	uint32_t							obj_type;						/* 0x000C */
	nwn_objid_t							obj_id;								/* 0x0004 */
	uint8_t								obj_type2;							/* 0x0008 */
	
	uint8_t								field_0009;							/* 0x0009 */
	uint8_t								field_000A;							/* 0x000A */
	uint8_t								field_000B;							/* 0x000B */
	
	void				*obj_vtable;						/* 0x000C */
	char								*obj_lastname;						/* 0x0010 */
	
	uint32_t							field_0014;							/* 0x0014 */
	
	CExoString							obj_tag;							/* 0x0018 */
//	uint32_t							obj_tag_len;						/* 0x001C */

	CExoString							obj_template;						/* 0x0020 */
//	uint32_t							obj_template_len;					/* 0x0024 */

};

#endif /* _NX_NWN_STRUCT_CGENERICOBJECT_ */
	

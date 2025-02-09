/**
 * NEURON IIoT System for Industry 4.0
 * Copyright (C) 2020-2023 EMQ Technologies Co., Ltd All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

#include <stdlib.h>
#include <string.h>

#include <jansson.h>

#include "json/json.h"

#include "neu_json_ndriver.h"

int neu_json_encode_ndriver_map(void *json_object, void *param)
{
    int                     ret = 0;
    neu_json_ndriver_map_t *req = param;

    neu_json_elem_t req_elems[] = {
        {
            .name      = "ndriver",
            .t         = NEU_JSON_STR,
            .v.val_str = req->ndriver,
        },
        {
            .name      = "driver",
            .t         = NEU_JSON_STR,
            .v.val_str = req->driver,
        },
        {
            .name      = "group",
            .t         = NEU_JSON_STR,
            .v.val_str = req->group,
        },
    };

    ret = neu_json_encode_field(json_object, req_elems,
                                NEU_JSON_ELEM_SIZE(req_elems));

    return ret;
}

void neu_json_decode_ndriver_map_free(neu_json_ndriver_map_t *req);

int neu_json_decode_ndriver_map(char *buf, neu_json_ndriver_map_t **result)
{
    int                     ret      = 0;
    void *                  json_obj = NULL;
    neu_json_ndriver_map_t *req      = NULL;

    json_obj = neu_json_decode_new(buf);
    if (NULL == json_obj) {
        return -1;
    }

    req = calloc(1, sizeof(*req));
    if (req == NULL) {
        goto error;
    }

    neu_json_elem_t req_elems[] = {
        {
            .name = "ndriver",
            .t    = NEU_JSON_STR,
        },
        {
            .name = "driver",
            .t    = NEU_JSON_STR,
        },
        {
            .name = "group",
            .t    = NEU_JSON_STR,
        },
    };
    ret = neu_json_decode_by_json(json_obj, NEU_JSON_ELEM_SIZE(req_elems),
                                  req_elems);
    req->ndriver = req_elems[0].v.val_str;
    req->driver  = req_elems[1].v.val_str;
    req->group   = req_elems[2].v.val_str;

    if (ret != 0) {
        goto error;
    }

    *result = req;
    neu_json_decode_free(json_obj);
    return 0;

error:
    neu_json_decode_ndriver_map_free(req);
    neu_json_decode_free(json_obj);
    return -1;
}

void neu_json_decode_ndriver_map_free(neu_json_ndriver_map_t *req)
{
    if (req) {
        free(req->ndriver);
        free(req->driver);
        free(req->group);
        free(req);
    }
}

int neu_json_encode_ndriver_group_array(void *json_arr, void *param)
{
    neu_json_ndriver_map_group_array_t *resp =
        (neu_json_ndriver_map_group_array_t *) param;

    for (int i = 0; i < resp->n_group; i++) {
        json_t *ob = json_object();

        if (0 !=
                json_object_set_new(ob, "driver",
                                    json_string(resp->groups[i].driver)) ||
            0 !=
                json_object_set_new(ob, "group",
                                    json_string(resp->groups[i].group)) ||
            0 != json_array_append_new(json_arr, ob)) {
            json_decref(ob);
            return -1;
        }
    }

    return 0;
}

int neu_json_encode_get_ndriver_maps_resp(void *json_obj, void *param)
{
    int ret = 0;

    void *group_array = neu_json_array();
    if (NULL == group_array) {
        return -1;
    }

    if (0 != neu_json_encode_ndriver_group_array(group_array, param)) {
        neu_json_encode_free(group_array);
        return -1;
    }

    neu_json_elem_t resp_elems[] = { {
        .name         = "groups",
        .t            = NEU_JSON_OBJECT,
        .v.val_object = group_array,
    } };
    ret                          = neu_json_encode_field(json_obj, resp_elems,
                                NEU_JSON_ELEM_SIZE(resp_elems));

    return ret;
}

int neu_json_encode_ndriver_tag(void *json_obj, void *param)
{
    int                     ret = 0;
    neu_json_ndriver_tag_t *tag = param;

    neu_json_elem_t elems[] = {
        {
            .name      = "name",
            .t         = NEU_JSON_STR,
            .v.val_str = tag->name,
        },
        {
            .name      = "address",
            .t         = NEU_JSON_STR,
            .v.val_str = tag->address,
        },
        {
            .name      = "attribute",
            .t         = NEU_JSON_INT,
            .v.val_int = tag->attribute,
        },
        {
            .name      = "type",
            .t         = NEU_JSON_INT,
            .v.val_int = tag->type,
        },
    };

    ret = neu_json_encode_field(json_obj, elems, NEU_JSON_ELEM_SIZE(elems));
    if (0 != ret) {
        return -1;
    }

    neu_json_elem_t params_elem[] = {
        {
            .name      = "params",
            .t         = NEU_JSON_STR,
            .v.val_str = tag->params,
        },
    };
    ret = neu_json_encode_field(json_obj, params_elem,
                                NEU_JSON_ELEM_SIZE(params_elem));

    return ret;
}

int neu_json_encode_ndriver_tag_array(void *json_obj, void *param)
{
    neu_json_ndriver_tag_array_t *array = param;

    if (!json_is_array((json_t *) json_obj)) {
        return -1;
    }

    for (int i = 0; i < array->len; i++) {
        json_t *tag_obj = json_object();
        if (NULL == tag_obj || 0 != json_array_append_new(json_obj, tag_obj)) {
            return -1;
        }
        if (0 != neu_json_encode_ndriver_tag(tag_obj, &array->data[i])) {
            return -1;
        }
    }

    return 0;
}

int neu_json_encode_get_ndriver_tags_resp(void *json_obj, void *param)
{
    int ret = 0;

    void *tags_array = neu_json_array();
    if (NULL == tags_array) {
        return -1;
    }

    if (0 != neu_json_encode_ndriver_tag_array(tags_array, param)) {
        neu_json_encode_free(tags_array);
        return -1;
    }

    neu_json_elem_t resp_elems[] = { {
        .name         = "tags",
        .t            = NEU_JSON_OBJECT,
        .v.val_object = tags_array,
    } };
    ret                          = neu_json_encode_field(json_obj, resp_elems,
                                NEU_JSON_ELEM_SIZE(resp_elems));

    return ret;
}

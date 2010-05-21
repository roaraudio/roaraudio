//meta.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

/*

grep ^'#define ROAR_META_TYPE_' meta.h | cut -d' ' -f2 | while read line; do printf ' {%-30s     "%-16s},\n' $line, $(echo $line | cut -d_ -f4)\"; done

*/

struct {
 int    id;
 char * name;
} _libroar_meta_typelist[] = {
 {ROAR_META_TYPE_NONE,               "NONE"           },
 {ROAR_META_TYPE_TITLE,              "TITLE"          },
 {ROAR_META_TYPE_ALBUM,              "ALBUM"          },
 {ROAR_META_TYPE_AUTHOR,             "AUTHOR"         },
 {ROAR_META_TYPE_AUTOR,              "AUTOR"          },
 {ROAR_META_TYPE_ARTIST,             "ARTIST"         },
 {ROAR_META_TYPE_VERSION,            "VERSION"        },
 {ROAR_META_TYPE_DATE,               "DATE"           },
 {ROAR_META_TYPE_LICENSE,            "LICENSE"        },
 {ROAR_META_TYPE_TRACKNUMBER,        "TRACKNUMBER"    },
 {ROAR_META_TYPE_ORGANIZATION,       "ORGANIZATION"   },
 {ROAR_META_TYPE_DESCRIPTION,        "DESCRIPTION"    },
 {ROAR_META_TYPE_GENRE,              "GENRE"          },
 {ROAR_META_TYPE_LOCATION,           "LOCATION"       },
 {ROAR_META_TYPE_CONTACT,            "CONTACT"        },
 {ROAR_META_TYPE_STREAMURL,          "STREAMURL"      },
 {ROAR_META_TYPE_HOMEPAGE,           "HOMEPAGE"       },
 {ROAR_META_TYPE_THUMBNAIL,          "THUMBNAIL"      },
 {ROAR_META_TYPE_LENGTH,             "LENGTH"         },
 {ROAR_META_TYPE_COMMENT,            "COMMENT"        },
 {ROAR_META_TYPE_OTHER,              "OTHER"          },
 {ROAR_META_TYPE_FILENAME,           "FILENAME"       },
 {ROAR_META_TYPE_FILEURL,            "FILEURL"        },
 {ROAR_META_TYPE_SERVER,             "SERVER"         },
 {ROAR_META_TYPE_DURATION,           "DURATION"       },
 {ROAR_META_TYPE_WWW,                "WWW"            },
 {ROAR_META_TYPE_WOAF,               "WOAF"           },
 {ROAR_META_TYPE_ENCODER,            "ENCODER"        },
 {ROAR_META_TYPE_ENCODEDBY,          "ENCODEDBY"      },
 {ROAR_META_TYPE_YEAR,               "YEAR"           },
 {ROAR_META_TYPE_DISCID,             "DISCID"         },
 {ROAR_META_TYPE_RPG_TRACK_PEAK,     "REPLAYGAIN_TRACK_PEAK" },
 {ROAR_META_TYPE_RPG_TRACK_GAIN,     "REPLAYGAIN_TRACK_GAIN" },
 {ROAR_META_TYPE_RPG_ALBUM_PEAK,     "REPLAYGAIN_ALBUM_PEAK" },
 {ROAR_META_TYPE_RPG_ALBUM_GAIN,     "REPLAYGAIN_ALBUM_GAIN" },
 {ROAR_META_TYPE_HASH,               "HASH"           },
 {ROAR_META_TYPE_SIGNALINFO,         "SIGNALINFO"     },
 {ROAR_META_TYPE_AUDIOINFO,          "AUDIOINFO"      },
 {ROAR_META_TYPE_OFFSET,             "OFFSET"         },
 {ROAR_META_TYPE_PERFORMER,          "PERFORMER"      },
 {ROAR_META_TYPE_COPYRIGHT,          "COPYRIGHT"      },
 
 {-1, "EOL"}
};


struct {
 int    id;
 char * name;
} _libroar_meta_genrelist[] = {
{ROAR_META_GENRE_RDS_EU_NONE,             "rds_eu_none"},
{ROAR_META_GENRE_RDS_EU_NONE,             "none"},
{ROAR_META_GENRE_RDS_EU_NEWS,             "rds_eu_news"},
{ROAR_META_GENRE_RDS_EU_NEWS,             "news"},
{ROAR_META_GENRE_RDS_EU_CURRENT_AFFAIRS,  "rds_eu_current_affairs"},
{ROAR_META_GENRE_RDS_EU_CURRENT_AFFAIRS,  "current affairs"},
{ROAR_META_GENRE_RDS_EU_INFORMATION,      "rds_eu_information"},
{ROAR_META_GENRE_RDS_EU_INFORMATION,      "information"},
{ROAR_META_GENRE_RDS_EU_SPORT,            "rds_eu_sport"},
{ROAR_META_GENRE_RDS_EU_SPORT,            "sport"},
{ROAR_META_GENRE_RDS_EU_EDUCATION,        "rds_eu_education"},
{ROAR_META_GENRE_RDS_EU_EDUCATION,        "education"},
{ROAR_META_GENRE_RDS_EU_DRAMA,            "rds_eu_drama"},
{ROAR_META_GENRE_RDS_EU_DRAMA,            "drama"},
{ROAR_META_GENRE_RDS_EU_CULTURE,          "rds_eu_culture"},
{ROAR_META_GENRE_RDS_EU_CULTURE,          "culture"},
{ROAR_META_GENRE_RDS_EU_SCIENCE,          "rds_eu_science"},
{ROAR_META_GENRE_RDS_EU_SCIENCE,          "science"},
{ROAR_META_GENRE_RDS_EU_VARIED,           "rds_eu_varied"},
{ROAR_META_GENRE_RDS_EU_VARIED,           "varied"},
{ROAR_META_GENRE_RDS_EU_POP_MUSIC,        "rds_eu_pop_music"},
{ROAR_META_GENRE_RDS_EU_POP_MUSIC,        "pop music"},
{ROAR_META_GENRE_RDS_EU_ROCK_MUSIC,       "rds_eu_rock_music"},
{ROAR_META_GENRE_RDS_EU_ROCK_MUSIC,       "rock music"},
{ROAR_META_GENRE_RDS_EU_EASY_LISTENING,   "rds_eu_easy_listening"},
{ROAR_META_GENRE_RDS_EU_EASY_LISTENING,   "easy listening"},
{ROAR_META_GENRE_RDS_EU_LIGHT_CLASSICAL,  "rds_eu_light_classical"},
{ROAR_META_GENRE_RDS_EU_LIGHT_CLASSICAL,  "light classical"},
{ROAR_META_GENRE_RDS_EU_SERIOUS_CLASSICAL, "rds_eu_serious_classical"},
{ROAR_META_GENRE_RDS_EU_SERIOUS_CLASSICAL, "serious classical"},
{ROAR_META_GENRE_RDS_EU_OTHER_MUSIC,      "rds_eu_other_music"},
{ROAR_META_GENRE_RDS_EU_OTHER_MUSIC,      "other music"},
{ROAR_META_GENRE_RDS_EU_WEATHER,          "rds_eu_weather"},
{ROAR_META_GENRE_RDS_EU_WEATHER,          "weather"},
{ROAR_META_GENRE_RDS_EU_FINANCE,          "rds_eu_finance"},
{ROAR_META_GENRE_RDS_EU_FINANCE,          "finance"},
{ROAR_META_GENRE_RDS_EU_CHILDREN_S_PROGRAMMES, "rds_eu_children_s_programmes"},
{ROAR_META_GENRE_RDS_EU_CHILDREN_S_PROGRAMMES, "children s programmes"},
{ROAR_META_GENRE_RDS_EU_SOCIAL_AFFAIRS,   "rds_eu_social_affairs"},
{ROAR_META_GENRE_RDS_EU_SOCIAL_AFFAIRS,   "social affairs"},
{ROAR_META_GENRE_RDS_EU_RELIGION,         "rds_eu_religion"},
{ROAR_META_GENRE_RDS_EU_RELIGION,         "religion"},
{ROAR_META_GENRE_RDS_EU_PHONE_IN,         "rds_eu_phone_in"},
{ROAR_META_GENRE_RDS_EU_PHONE_IN,         "phone in"},
{ROAR_META_GENRE_RDS_EU_TRAVEL,           "rds_eu_travel"},
{ROAR_META_GENRE_RDS_EU_TRAVEL,           "travel"},
{ROAR_META_GENRE_RDS_EU_LEISURE,          "rds_eu_leisure"},
{ROAR_META_GENRE_RDS_EU_LEISURE,          "leisure"},
{ROAR_META_GENRE_RDS_EU_JAZZ_MUSIC,       "rds_eu_jazz_music"},
{ROAR_META_GENRE_RDS_EU_JAZZ_MUSIC,       "jazz music"},
{ROAR_META_GENRE_RDS_EU_COUNTRY_MUSIC,    "rds_eu_country_music"},
{ROAR_META_GENRE_RDS_EU_COUNTRY_MUSIC,    "country music"},
{ROAR_META_GENRE_RDS_EU_NATIONAL_MUSIC,   "rds_eu_national_music"},
{ROAR_META_GENRE_RDS_EU_NATIONAL_MUSIC,   "national music"},
{ROAR_META_GENRE_RDS_EU_OLDIES_MUSIC,     "rds_eu_oldies_music"},
{ROAR_META_GENRE_RDS_EU_OLDIES_MUSIC,     "oldies music"},
{ROAR_META_GENRE_RDS_EU_FOLK_MUSIC,       "rds_eu_folk_music"},
{ROAR_META_GENRE_RDS_EU_FOLK_MUSIC,       "folk music"},
{ROAR_META_GENRE_RDS_EU_DOCUMENTARY,      "rds_eu_documentary"},
{ROAR_META_GENRE_RDS_EU_DOCUMENTARY,      "documentary"},
{ROAR_META_GENRE_RDS_EU_ALARM_TEST,       "rds_eu_alarm_test"},
{ROAR_META_GENRE_RDS_EU_ALARM_TEST,       "alarm test"},
{ROAR_META_GENRE_RDS_EU_ALARM,            "rds_eu_alarm"},
{ROAR_META_GENRE_RDS_EU_ALARM,            "alarm"},
{ROAR_META_GENRE_RDS_EU_EMERGENCY_TEST,   "rds_eu_emergency_test"},
{ROAR_META_GENRE_RDS_EU_EMERGENCY_TEST,   "emergency test"},
{ROAR_META_GENRE_RDS_EU_EMERGENCY,        "rds_eu_emergency"},
{ROAR_META_GENRE_RDS_EU_EMERGENCY,        "emergency"},
{ROAR_META_GENRE_RDS_NA_NONE,             "rds_na_none"},
{ROAR_META_GENRE_RDS_NA_NONE,             "none"},
{ROAR_META_GENRE_RDS_NA_NEWS,             "rds_na_news"},
{ROAR_META_GENRE_RDS_NA_NEWS,             "news"},
{ROAR_META_GENRE_RDS_NA_INFORMATION,      "rds_na_information"},
{ROAR_META_GENRE_RDS_NA_INFORMATION,      "information"},
{ROAR_META_GENRE_RDS_NA_SPORTS,           "rds_na_sports"},
{ROAR_META_GENRE_RDS_NA_SPORTS,           "sports"},
{ROAR_META_GENRE_RDS_NA_TALK,             "rds_na_talk"},
{ROAR_META_GENRE_RDS_NA_TALK,             "talk"},
{ROAR_META_GENRE_RDS_NA_ROCK,             "rds_na_rock"},
{ROAR_META_GENRE_RDS_NA_ROCK,             "rock"},
{ROAR_META_GENRE_RDS_NA_CLASSIC_ROCK,     "rds_na_classic_rock"},
{ROAR_META_GENRE_RDS_NA_CLASSIC_ROCK,     "classic rock"},
{ROAR_META_GENRE_RDS_NA_ADULT_HITS,       "rds_na_adult_hits"},
{ROAR_META_GENRE_RDS_NA_ADULT_HITS,       "adult hits"},
{ROAR_META_GENRE_RDS_NA_SOFT_ROCK,        "rds_na_soft_rock"},
{ROAR_META_GENRE_RDS_NA_SOFT_ROCK,        "soft rock"},
{ROAR_META_GENRE_RDS_NA_TOP_40,           "rds_na_top_40"},
{ROAR_META_GENRE_RDS_NA_TOP_40,           "top 40"},
{ROAR_META_GENRE_RDS_NA_COUNTRY,          "rds_na_country"},
{ROAR_META_GENRE_RDS_NA_COUNTRY,          "country"},
{ROAR_META_GENRE_RDS_NA_OLDIES,           "rds_na_oldies"},
{ROAR_META_GENRE_RDS_NA_OLDIES,           "oldies"},
{ROAR_META_GENRE_RDS_NA_SOFT,             "rds_na_soft"},
{ROAR_META_GENRE_RDS_NA_SOFT,             "soft"},
{ROAR_META_GENRE_RDS_NA_NOSTALGIA,        "rds_na_nostalgia"},
{ROAR_META_GENRE_RDS_NA_NOSTALGIA,        "nostalgia"},
{ROAR_META_GENRE_RDS_NA_JAZZ,             "rds_na_jazz"},
{ROAR_META_GENRE_RDS_NA_JAZZ,             "jazz"},
{ROAR_META_GENRE_RDS_NA_CLASSICAL,        "rds_na_classical"},
{ROAR_META_GENRE_RDS_NA_CLASSICAL,        "classical"},
{ROAR_META_GENRE_RDS_NA_RHYTHM_AND_BLUES, "rds_na_rhythm_and_blues"},
{ROAR_META_GENRE_RDS_NA_RHYTHM_AND_BLUES, "rhythm and blues"},
{ROAR_META_GENRE_RDS_NA_SOFT_RHYTHM_AND_BLUES, "rds_na_soft_rhythm_and_blues"},
{ROAR_META_GENRE_RDS_NA_SOFT_RHYTHM_AND_BLUES, "soft rhythm and blues"},
{ROAR_META_GENRE_RDS_NA_LANGUAGE,         "rds_na_language"},
{ROAR_META_GENRE_RDS_NA_LANGUAGE,         "language"},
{ROAR_META_GENRE_RDS_NA_RELIGIOUS_MUSIC,  "rds_na_religious_music"},
{ROAR_META_GENRE_RDS_NA_RELIGIOUS_MUSIC,  "religious music"},
{ROAR_META_GENRE_RDS_NA_RELIGIOUS_TALK,   "rds_na_religious_talk"},
{ROAR_META_GENRE_RDS_NA_RELIGIOUS_TALK,   "religious talk"},
{ROAR_META_GENRE_RDS_NA_PERSONALITY,      "rds_na_personality"},
{ROAR_META_GENRE_RDS_NA_PERSONALITY,      "personality"},
{ROAR_META_GENRE_RDS_NA_PUBLIC,           "rds_na_public"},
{ROAR_META_GENRE_RDS_NA_PUBLIC,           "public"},
{ROAR_META_GENRE_RDS_NA_COLLEGE,          "rds_na_college"},
{ROAR_META_GENRE_RDS_NA_COLLEGE,          "college"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_0,     "rds_na_unassigned_0"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_0,     "unassigned 0"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_1,     "rds_na_unassigned_1"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_1,     "unassigned 1"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_2,     "rds_na_unassigned_2"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_2,     "unassigned 2"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_3,     "rds_na_unassigned_3"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_3,     "unassigned 3"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_4,     "rds_na_unassigned_4"},
{ROAR_META_GENRE_RDS_NA_UNASSIGNED_4,     "unassigned 4"},
{ROAR_META_GENRE_RDS_NA_WEATHER,          "rds_na_weather"},
{ROAR_META_GENRE_RDS_NA_WEATHER,          "weather"},
{ROAR_META_GENRE_RDS_NA_EMERGENCY_TEST,   "rds_na_emergency_test"},
{ROAR_META_GENRE_RDS_NA_EMERGENCY_TEST,   "emergency test"},
{ROAR_META_GENRE_RDS_NA_EMERGENCY,        "rds_na_emergency"},
{ROAR_META_GENRE_RDS_NA_EMERGENCY,        "emergency"},
{ROAR_META_GENRE_RDS_NA_ALARM_TEST,       "rds_na_alarm_test"},
{ROAR_META_GENRE_RDS_NA_ALARM_TEST,       "alarm test"},
{ROAR_META_GENRE_RDS_NA_ALARM,            "rds_na_alarm"},
{ROAR_META_GENRE_RDS_NA_ALARM,            "alarm"},
{ROAR_META_GENRE_ID3_BLUES,               "id3_blues"},
{ROAR_META_GENRE_ID3_BLUES,               "blues"},
{ROAR_META_GENRE_ID3_CLASSIC_ROCK,        "id3_classic_rock"},
{ROAR_META_GENRE_ID3_CLASSIC_ROCK,        "classic rock"},
{ROAR_META_GENRE_ID3_COUNTRY,             "id3_country"},
{ROAR_META_GENRE_ID3_COUNTRY,             "country"},
{ROAR_META_GENRE_ID3_DANCE,               "id3_dance"},
{ROAR_META_GENRE_ID3_DANCE,               "dance"},
{ROAR_META_GENRE_ID3_DISCO,               "id3_disco"},
{ROAR_META_GENRE_ID3_DISCO,               "disco"},
{ROAR_META_GENRE_ID3_FUNK,                "id3_funk"},
{ROAR_META_GENRE_ID3_FUNK,                "funk"},
{ROAR_META_GENRE_ID3_GRUNGE,              "id3_grunge"},
{ROAR_META_GENRE_ID3_GRUNGE,              "grunge"},
{ROAR_META_GENRE_ID3_HIP_HOP,             "id3_hip_hop"},
{ROAR_META_GENRE_ID3_HIP_HOP,             "hip hop"},
{ROAR_META_GENRE_ID3_JAZZ,                "id3_jazz"},
{ROAR_META_GENRE_ID3_JAZZ,                "jazz"},
{ROAR_META_GENRE_ID3_METAL,               "id3_metal"},
{ROAR_META_GENRE_ID3_METAL,               "metal"},
{ROAR_META_GENRE_ID3_NEW_AGE,             "id3_new_age"},
{ROAR_META_GENRE_ID3_NEW_AGE,             "new age"},
{ROAR_META_GENRE_ID3_OLDIES,              "id3_oldies"},
{ROAR_META_GENRE_ID3_OLDIES,              "oldies"},
{ROAR_META_GENRE_ID3_OTHER,               "id3_other"},
{ROAR_META_GENRE_ID3_OTHER,               "other"},
{ROAR_META_GENRE_ID3_POP,                 "id3_pop"},
{ROAR_META_GENRE_ID3_POP,                 "pop"},
{ROAR_META_GENRE_ID3_R_AND_B,             "id3_r_and_b"},
{ROAR_META_GENRE_ID3_R_AND_B,             "r and b"},
{ROAR_META_GENRE_ID3_RAP,                 "id3_rap"},
{ROAR_META_GENRE_ID3_RAP,                 "rap"},
{ROAR_META_GENRE_ID3_REGGAE,              "id3_reggae"},
{ROAR_META_GENRE_ID3_REGGAE,              "reggae"},
{ROAR_META_GENRE_ID3_ROCK,                "id3_rock"},
{ROAR_META_GENRE_ID3_ROCK,                "rock"},
{ROAR_META_GENRE_ID3_TECHNO,              "id3_techno"},
{ROAR_META_GENRE_ID3_TECHNO,              "techno"},
{ROAR_META_GENRE_ID3_INDUSTRIAL,          "id3_industrial"},
{ROAR_META_GENRE_ID3_INDUSTRIAL,          "industrial"},
{ROAR_META_GENRE_ID3_ALTERNATIVE,         "id3_alternative"},
{ROAR_META_GENRE_ID3_ALTERNATIVE,         "alternative"},
{ROAR_META_GENRE_ID3_SKA,                 "id3_ska"},
{ROAR_META_GENRE_ID3_SKA,                 "ska"},
{ROAR_META_GENRE_ID3_DEATH_METAL,         "id3_death_metal"},
{ROAR_META_GENRE_ID3_DEATH_METAL,         "death metal"},
{ROAR_META_GENRE_ID3_PRANKS,              "id3_pranks"},
{ROAR_META_GENRE_ID3_PRANKS,              "pranks"},
{ROAR_META_GENRE_ID3_SOUNDTRACK,          "id3_soundtrack"},
{ROAR_META_GENRE_ID3_SOUNDTRACK,          "soundtrack"},
{ROAR_META_GENRE_ID3_EURO_TECHNO,         "id3_euro_techno"},
{ROAR_META_GENRE_ID3_EURO_TECHNO,         "euro techno"},
{ROAR_META_GENRE_ID3_AMBIENT,             "id3_ambient"},
{ROAR_META_GENRE_ID3_AMBIENT,             "ambient"},
{ROAR_META_GENRE_ID3_TRIP_HOP,            "id3_trip_hop"},
{ROAR_META_GENRE_ID3_TRIP_HOP,            "trip hop"},
{ROAR_META_GENRE_ID3_VOCAL,               "id3_vocal"},
{ROAR_META_GENRE_ID3_VOCAL,               "vocal"},
{ROAR_META_GENRE_ID3_JAZZ_FUNK,           "id3_jazz_funk"},
{ROAR_META_GENRE_ID3_JAZZ_FUNK,           "jazz funk"},
{ROAR_META_GENRE_ID3_FUSION,              "id3_fusion"},
{ROAR_META_GENRE_ID3_FUSION,              "fusion"},
{ROAR_META_GENRE_ID3_TRANCE,              "id3_trance"},
{ROAR_META_GENRE_ID3_TRANCE,              "trance"},
{ROAR_META_GENRE_ID3_CLASSICAL,           "id3_classical"},
{ROAR_META_GENRE_ID3_CLASSICAL,           "classical"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL,        "id3_instrumental"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL,        "instrumental"},
{ROAR_META_GENRE_ID3_ACID,                "id3_acid"},
{ROAR_META_GENRE_ID3_ACID,                "acid"},
{ROAR_META_GENRE_ID3_HOUSE,               "id3_house"},
{ROAR_META_GENRE_ID3_HOUSE,               "house"},
{ROAR_META_GENRE_ID3_GAME,                "id3_game"},
{ROAR_META_GENRE_ID3_GAME,                "game"},
{ROAR_META_GENRE_ID3_SOUND_CLIP,          "id3_sound_clip"},
{ROAR_META_GENRE_ID3_SOUND_CLIP,          "sound clip"},
{ROAR_META_GENRE_ID3_GOSPEL,              "id3_gospel"},
{ROAR_META_GENRE_ID3_GOSPEL,              "gospel"},
{ROAR_META_GENRE_ID3_NOISE,               "id3_noise"},
{ROAR_META_GENRE_ID3_NOISE,               "noise"},
{ROAR_META_GENRE_ID3_ALTERNROCK,          "id3_alternrock"},
{ROAR_META_GENRE_ID3_ALTERNROCK,          "alternrock"},
{ROAR_META_GENRE_ID3_BASS,                "id3_bass"},
{ROAR_META_GENRE_ID3_BASS,                "bass"},
{ROAR_META_GENRE_ID3_SOUL,                "id3_soul"},
{ROAR_META_GENRE_ID3_SOUL,                "soul"},
{ROAR_META_GENRE_ID3_PUNK,                "id3_punk"},
{ROAR_META_GENRE_ID3_PUNK,                "punk"},
{ROAR_META_GENRE_ID3_SPACE,               "id3_space"},
{ROAR_META_GENRE_ID3_SPACE,               "space"},
{ROAR_META_GENRE_ID3_MEDITATIVE,          "id3_meditative"},
{ROAR_META_GENRE_ID3_MEDITATIVE,          "meditative"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL_POP,    "id3_instrumental_pop"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL_POP,    "instrumental pop"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL_ROCK,   "id3_instrumental_rock"},
{ROAR_META_GENRE_ID3_INSTRUMENTAL_ROCK,   "instrumental rock"},
{ROAR_META_GENRE_ID3_ETHNIC,              "id3_ethnic"},
{ROAR_META_GENRE_ID3_ETHNIC,              "ethnic"},
{ROAR_META_GENRE_ID3_GOTHIC,              "id3_gothic"},
{ROAR_META_GENRE_ID3_GOTHIC,              "gothic"},
{ROAR_META_GENRE_ID3_DARKWAVE,            "id3_darkwave"},
{ROAR_META_GENRE_ID3_DARKWAVE,            "darkwave"},
{ROAR_META_GENRE_ID3_TECHNO_INDUSTRIAL,   "id3_techno_industrial"},
{ROAR_META_GENRE_ID3_TECHNO_INDUSTRIAL,   "techno industrial"},
{ROAR_META_GENRE_ID3_ELECTRONIC,          "id3_electronic"},
{ROAR_META_GENRE_ID3_ELECTRONIC,          "electronic"},
{ROAR_META_GENRE_ID3_POP_FOLK,            "id3_pop_folk"},
{ROAR_META_GENRE_ID3_POP_FOLK,            "pop folk"},
{ROAR_META_GENRE_ID3_EURODANCE,           "id3_eurodance"},
{ROAR_META_GENRE_ID3_EURODANCE,           "eurodance"},
{ROAR_META_GENRE_ID3_DREAM,               "id3_dream"},
{ROAR_META_GENRE_ID3_DREAM,               "dream"},
{ROAR_META_GENRE_ID3_SOUTHERN_ROCK,       "id3_southern_rock"},
{ROAR_META_GENRE_ID3_SOUTHERN_ROCK,       "southern rock"},
{ROAR_META_GENRE_ID3_COMEDY,              "id3_comedy"},
{ROAR_META_GENRE_ID3_COMEDY,              "comedy"},
{ROAR_META_GENRE_ID3_CULT,                "id3_cult"},
{ROAR_META_GENRE_ID3_CULT,                "cult"},
{ROAR_META_GENRE_ID3_GANGSTA,             "id3_gangsta"},
{ROAR_META_GENRE_ID3_GANGSTA,             "gangsta"},
{ROAR_META_GENRE_ID3_TOP_40,              "id3_top_40"},
{ROAR_META_GENRE_ID3_TOP_40,              "top 40"},
{ROAR_META_GENRE_ID3_CHRISTIAN_RAP,       "id3_christian_rap"},
{ROAR_META_GENRE_ID3_CHRISTIAN_RAP,       "christian rap"},
{ROAR_META_GENRE_ID3_POP_FUNK,            "id3_pop_funk"},
{ROAR_META_GENRE_ID3_POP_FUNK,            "pop funk"},
{ROAR_META_GENRE_ID3_JUNGLE,              "id3_jungle"},
{ROAR_META_GENRE_ID3_JUNGLE,              "jungle"},
{ROAR_META_GENRE_ID3_NATIVE_AMERICAN,     "id3_native_american"},
{ROAR_META_GENRE_ID3_NATIVE_AMERICAN,     "native american"},
{ROAR_META_GENRE_ID3_CABARET,             "id3_cabaret"},
{ROAR_META_GENRE_ID3_CABARET,             "cabaret"},
{ROAR_META_GENRE_ID3_NEW_WAVE,            "id3_new_wave"},
{ROAR_META_GENRE_ID3_NEW_WAVE,            "new wave"},
{ROAR_META_GENRE_ID3_PSYCHADELIC,         "id3_psychadelic"},
{ROAR_META_GENRE_ID3_PSYCHADELIC,         "psychadelic"},
{ROAR_META_GENRE_ID3_RAVE,                "id3_rave"},
{ROAR_META_GENRE_ID3_RAVE,                "rave"},
{ROAR_META_GENRE_ID3_SHOWTUNES,           "id3_showtunes"},
{ROAR_META_GENRE_ID3_SHOWTUNES,           "showtunes"},
{ROAR_META_GENRE_ID3_TRAILER,             "id3_trailer"},
{ROAR_META_GENRE_ID3_TRAILER,             "trailer"},
{ROAR_META_GENRE_ID3_LO_FI,               "id3_lo_fi"},
{ROAR_META_GENRE_ID3_LO_FI,               "lo fi"},
{ROAR_META_GENRE_ID3_TRIBAL,              "id3_tribal"},
{ROAR_META_GENRE_ID3_TRIBAL,              "tribal"},
{ROAR_META_GENRE_ID3_ACID_PUNK,           "id3_acid_punk"},
{ROAR_META_GENRE_ID3_ACID_PUNK,           "acid punk"},
{ROAR_META_GENRE_ID3_ACID_JAZZ,           "id3_acid_jazz"},
{ROAR_META_GENRE_ID3_ACID_JAZZ,           "acid jazz"},
{ROAR_META_GENRE_ID3_POLKA,               "id3_polka"},
{ROAR_META_GENRE_ID3_POLKA,               "polka"},
{ROAR_META_GENRE_ID3_RETRO,               "id3_retro"},
{ROAR_META_GENRE_ID3_RETRO,               "retro"},
{ROAR_META_GENRE_ID3_MUSICAL,             "id3_musical"},
{ROAR_META_GENRE_ID3_MUSICAL,             "musical"},
{ROAR_META_GENRE_ID3_ROCK_AND_ROLL,       "id3_rock_and_roll"},
{ROAR_META_GENRE_ID3_ROCK_AND_ROLL,       "rock and roll"},
{ROAR_META_GENRE_ID3_HARD_ROCK,           "id3_hard_rock"},
{ROAR_META_GENRE_ID3_HARD_ROCK,           "hard rock"},

 {-1, "EOL"}
};

char * roar_meta_strtype(int type) {
 int i;
 static char name[ROAR_META_MAX_NAMELEN];

 for (i = 0; _libroar_meta_typelist[i].id != -1; i++)
  if ( _libroar_meta_typelist[i].id == type ) {
   strncpy(name, _libroar_meta_typelist[i].name, ROAR_META_MAX_NAMELEN);
   return name;
  }

 return NULL;
}

int    roar_meta_inttype(char * type) {
 int i;

 for (i = 0; _libroar_meta_typelist[i].id != -1; i++)
  if ( strcasecmp(_libroar_meta_typelist[i].name, type) == 0 ) {
   return _libroar_meta_typelist[i].id;
  }

 return -1;
}

int roar_stream_meta_set (struct roar_connection * con, struct roar_stream * s, int mode, struct roar_meta * meta) {
 struct roar_message m;
 int len;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_SET_META;
 m.stream  = s->id;
// m.datalen = len;

 if ( (mode == ROAR_META_MODE_FINALIZE || mode == ROAR_META_MODE_CLEAR) && meta->value == NULL )
  meta->value = "";

 if ( meta->value == NULL )
   return -1;

 m.data[0] = 0;
 m.data[1] = mode;
 m.data[2] = meta->type;

 m.data[3] = strlen(meta->key);
 m.data[4] = len = strlen(meta->value);

 if ( len > 255 )
  return -1;

 m.datalen = (int) 5 + (int) m.data[3] + len;
 if ( m.datalen > LIBROAR_BUFFER_MSGDATA )
  return -1;

 strncpy(&(m.data[5]), meta->key, ROAR_META_MAX_NAMELEN);
 strncpy(&(m.data[5+m.data[3]]), meta->value, len);

 ROAR_DBG("roar_stream_meta_set(*): meta value length is %i byte", len);
 ROAR_DBG("roar_stream_meta_set(*): message data length is %i byte", m.datalen);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_meta_get (struct roar_connection * con, struct roar_stream * s, struct roar_meta * meta) {
 struct roar_message m;
 char * c;

 memset(&m,  0, sizeof(m));

 m.cmd     = ROAR_CMD_GET_META;
 m.stream  = s->id;
// m.datalen = len;

 m.data[0] = 0;
 m.data[1] = meta->type;
 m.datalen = 2;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( m.datalen < 2 )
  return -1;

 if ( m.data[0] != 0 )
  return -1;

 if ( (c = malloc(((unsigned) m.data[1]) + 1)) == NULL )
  return -1;

 strncpy(c, &(m.data[2]), (unsigned) m.data[1]);
 c[(unsigned) m.data[1]] = 0;

 meta->value  = c;
 meta->key[0] = 0;

 return 0;
}

int roar_stream_meta_list (struct roar_connection * con, struct roar_stream * s, int * types, size_t len) {
 int i;
 struct roar_message m;

 memset(&m,  0, sizeof(m));

 m.cmd     = ROAR_CMD_LIST_META;
 m.stream  = s->id;
 m.pos     = 0;

 m.data[0] = 0;
 m.datalen = 1;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( m.datalen < 1 )
  return -1;

 if ( m.data[0] != 0 )
  return -1;

 if ( len < (m.datalen - 1 ) )
  return -1;

 for (i = 1; i < m.datalen; i++)
  types[i-1] = (unsigned) m.data[i];

 return m.datalen - 1;
}

int roar_meta_free (struct roar_meta * meta) {
 if ( meta->value )
  free(meta->value);

 return 0;
}


// genere:
char * roar_meta_strgenre(int genre) {
 int i;

 for (i = 0; _libroar_meta_genrelist[i].id != -1; i++)
  if ( _libroar_meta_genrelist[i].id == genre ) {
   return _libroar_meta_genrelist[i].name;
  }

 return NULL;
}

int    roar_meta_intgenre(char * genre) {
 int i;

 for (i = 0; _libroar_meta_genrelist[i].id != -1; i++)
  if ( strcasecmp(_libroar_meta_genrelist[i].name, genre) == 0 ) {
   return _libroar_meta_genrelist[i].id;
  }

 return -1;
}

int    roar_meta_parse_audioinfo(struct roar_audio_info * info, char * str) {
 char * lc;
 char * cur, * next;
 char * k,   * v;

 if ( info == NULL || str == NULL )
  return -1;

 memset(info, 0, sizeof(struct roar_audio_info));

 info->codec = -1;

 if ( (lc = strdup(str)) == NULL )
  return -1;

 cur = lc;

 while (cur != NULL) {
  next = strstr(cur, " ");
  if ( next != NULL ) {
   *next = 0;
    next++;
  }

  k = cur;
  v = strstr(cur, ":");

  if ( v != NULL ) {
   *v = 0;
    v++;
  } else {
    v = "";
  }

  // we test for the key, unknown keys get ignored.
  if ( !strcasecmp(k, "rate") ) {
   info->rate     = atoi(v);
  } else if ( !strcasecmp(k, "bits") ) {
   info->bits     = atoi(v);
  } else if ( !strcasecmp(k, "channels") ) {
   info->channels = atoi(v);
  } else if ( !strcasecmp(k, "codec") ) {
   info->codec    = roar_str2codec(v);
  }

  cur = next;
 }

 free(lc);

 return 0;
}

//ll

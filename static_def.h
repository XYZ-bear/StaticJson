#pragma once
#include "static_json.h"

Json(Twitter)
{
public:
    Json(statuses_t){
    public:
        Json(metadata_t){
        public:
            string N(result_type);
            string N(iso_language_code);
        };
        metadata_t N(metadata);
        string N(created_at);
        int N(id);
        string N(id_str);
        string N(text);
        string N(source);
        bool N(truncated);
        Json(in_reply_to_status_id_t){
        public:
        };
        in_reply_to_status_id_t N(in_reply_to_status_id);
        Json(in_reply_to_status_id_str_t){
        public:
        };
        in_reply_to_status_id_str_t N(in_reply_to_status_id_str);
        int N(in_reply_to_user_id);
        string N(in_reply_to_user_id_str);
        string N(in_reply_to_screen_name);
        Json(user_t){
        public:
            int N(id);
            string N(id_str);
            string N(name);
            string N(screen_name);
            string N(location);
            string N(description);
            string N(url);
            Json(entities_t){
            public:
                Json(description_t){
                public:
                    vector<int> N(urls);
                };
                description_t N(description);
            };
            entities_t N(entities);
            bool N(protected1);
            int N(followers_count);
            int N(friends_count);
            int N(listed_count);
            string N(created_at);
            int N(favourites_count);
            Json(utc_offset_t){
            public:
            };
            utc_offset_t N(utc_offset);
            Json(time_zone_t){
            public:
            };
            time_zone_t N(time_zone);
            bool N(geo_enabled);
            bool N(verified);
            int N(statuses_count);
            string N(lang);
            bool N(contributors_enabled);
            bool N(is_translator);
            bool N(is_translation_enabled);
            string N(profile_background_color);
            string N(profile_background_image_url);
            string N(profile_background_image_url_https);
            bool N(profile_background_tile);
            string N(profile_image_url);
            string N(profile_image_url_https);
            string N(profile_banner_url);
            string N(profile_link_color);
            string N(profile_sidebar_border_color);
            string N(profile_sidebar_fill_color);
            string N(profile_text_color);
            bool N(profile_use_background_image);
            bool N(default_profile);
            bool N(default_profile_image);
            bool N(following);
            bool N(follow_request_sent);
            bool N(notifications);
        };
        user_t N(user);
        Json(geo_t){
        public:
        };
        geo_t N(geo);
        Json(coordinates_t){
        public:
        };
        coordinates_t N(coordinates);
        Json(place_t){
        public:
        };
        place_t N(place);
        Json(contributors_t){
        public:
        };
        contributors_t N(contributors);
        int N(retweet_count);
        int N(favorite_count);
        Json(entities_t){
        public:
            vector<int> N(hashtags);
            vector<int> N(symbols);
            vector<int> N(urls);
            Json(user_mentions_t){
            public:
                string N(screen_name);
                string N(name);
                int N(id);
                string N(id_str);
                vector<int> N(indices);
            };
            vector<user_mentions_t> N(user_mentions);
        };
        entities_t N(entities);
        bool N(favorited);
        bool N(retweeted);
        string N(lang);
    };
    vector<statuses_t> N(statuses);
    Json(search_metadata_t){
    public:
        double N(completed_in);
        int N(max_id);
        string N(max_id_str);
        string N(next_results);
        string N(query);
        string N(refresh_url);
        int N(count);
        int N(since_id);
        string N(since_id_str);
    };
    search_metadata_t N(search_metadata);
};

Json(Canada)
{
public:
    string N(type);
    Json(features_t){
    public:
        string N(type);
        Json(properties_t){
        public:
            string N(name);
        };
        properties_t N(properties);
        Json(geometry_t){
        public:
            string N(type);
            vector<vector<vector<double>>> N(coordinates);
        };
        geometry_t N(geometry);
    };
    vector<features_t> N(features);
};

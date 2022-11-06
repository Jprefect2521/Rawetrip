void Resolver::Initialize(Player* m_player, Record* m_record, float m_feet_yaw, float m_pitch)
{
    player = m_player;
    record = m_record;

    goalfeetyaw = Math::NormalizeYaw(m_feet_yaw);
    pitch = Math::NormalizePitch(m_pitch);
}

void Resolver::ResolvePitch()
{
    return pitch;
}

void Resolver::ResolveYaw()
{
    if (!local_player->alive() || !player->enemy())
    {
        player_record->side = RESOLVER_ORIGINAL;
        return;
    }

    bool BruteforceResolve = (record->resolver_type != LBY);

    if (BruteforceResolve)
    {
        switch (ResolverSide)
        {
        case RESOLVER_ORIGINAL:
            record->resolver_side = RESOLVER_ORIGINAL;
            record->resolver_type = BRUTEFORCE;
            break;
        case RESOLVER_ZERO:
            record->resolver_type = BRUTEFORCE;
            record->resolver_side = RESOLVER_LOW_POSITIVE;

            brutforced_first = false;
            brutforced_second = false;
            return;
        case RESOLVER_POSITIVE:
            record->resolver_type = BRUTEFORCE;
            int resolver_side;

            if (brutforced_second)
                resolver_side = RESOLVER_ZERO;
            else
                resolver_side = RESOLVER_NEGATIVE;

            record->resolver_side = resolver_side;
            brutforced_first = true;
            return;
        case RESOLVER_NEGATIVE:
            record->resolver_type = BRUTEFORCE;
            int resolver_side;

            if (brutforced_first)
                resolver_side = RESOLVER_ZERO;
            else
                resolver_side = RESOLVER_POSITIVE;

            record->resolver_side = resolver_side;
            brutforced_second = true;
            return;
        case RESOLVER_LOW_POSITIVE:
            record->resolver_type = BRUTEFORCE;
            record->resolver_side = RESOLVER_LOW_NEGATIVE;
            return;
        case RESOLVER_LOW_NEGATIVE:
            record->resolver_type = BRUTEFORCE;
            record->resolver_side = RESOLVER_POSITIVE;
            return;
        }
    }

    animstate* animation_state = player->get_animstate();

    if (!animation_state)
    {
        record->resolver_side = RESOLVER_ORIGINAL;
        return;
    }

    float delta = Math::NormalizeYaw(player->eye_angles().y - goalfeetyaw);

    if (delta > 30.0 && valid_lby)
    {
        if (globals.missed_shots > 0)
            delta = -delta;

        if (delta <= 30.0)
        {
            if (delta < -30.0)
            {
                record->resolver_type = LBY;
                record->resolver_side = RESOLVER_NEGATIVE;
            }
        }
        else
        {
            record->resolver_type = LBY;
            record->resolver_side = RESOLVER_POSITIVE;
        }
    }
    else
    {
        bool trace = false;

        if (globals.curtime - previous_curtime > 2.0f)
        {
            bool first = visible(globals.eye_pos, player->hitbox_pos(1, record->matrix_positive), player, localplayer);
            bool second = visible(globals.eye_pos, player->hitbox_pos(1, record->matrix_negative), player, localplayer);

            bool positive = false;

            if (first == second)
            {
                float first2 = globals.eye_pos.dist(player->hitbox_pos(1, record->matrix_positive));
                float second2 = globals.eye_pos.dist(player->hitbox_pos(1, record->matrix_negative));

                if (Math::SecondFabs(first2 - second2) > 1.0f)
                    positive = first2 > second2;
            }
            else
            {
                trace = true;
                positive = second;
                previous_curtime = globals.curtime;
            }
        }
        else
            trace = true;

        if (positive)
        {
            int type = ORIGINAL;

            if (trace)
                type = TRACE;
            else
                type = DIRECTIONAL;

            record->resolver_type = type;
            record->resolver_side = RESOLVER_POSITIVE;
        }
        else
        {
            int type = ORIGINAL;

            if (trace)
                type = TRACE;
            else
                type = DIRECTIONAL;

            record->resolver_type = type;
            record->resolver_side = RESOLVER_NEGATIVE;
        }
    }

    bool low_pitch = (animation_state->eye_pitch < 20.0 && animation_state->eye_pitch > -20.0);

    if (low_pitch)
    {
        float feet_yaw = animation_state->goalfeetyaw;

        if (animation_state)
        {
            float lby_delta = player->eye_angles().yaw - player->lby();
            bool lby_stand = lby_delta > 35.0 && player->velocity().length_2d() < 5;

            if (!lby_stand)
                feet_yaw += lby_delta;
            
            animation_state->goalfeetyaw = feet_yaw;
        }
    }
    else
    {
        record->resolver_side = RESOLVER_ORIGINAL;
    }
}
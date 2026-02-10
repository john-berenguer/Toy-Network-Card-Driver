void icmp_handle(const struct ip_header *ip,
                 const uint8_t *payload,
                 size_t len)
{
    if (len < sizeof(struct icmp_header))
        return;

    const struct icmp_header *icmp =
        (const struct icmp_header *)payload;

    if (icmp->type != ICMP_ECHO_REQUEST)
        return;

    uint8_t packet[1500];

    struct ip_header *ip_reply =
        (struct ip_header *)packet;

    uint8_t *icmp_reply =
        packet + sizeof(struct ip_header);

    memcpy(icmp_reply, payload, len);

    struct icmp_header *reply =
        (struct icmp_header *)icmp_reply;

    reply->type = ICMP_ECHO_REPLY;
    reply->code = 0;
    reply->checksum = 0;
    reply->checksum = icmp_checksum(icmp_reply, len);

    ip_reply->ver_ihl = 0x45;
    ip_reply->tos = 0;
    ip_reply->total_length =
        htons(sizeof(struct ip_header) + len);
    ip_reply->id = 0;
    ip_reply->flags_frag = 0;
    ip_reply->ttl = 64;
    ip_reply->protocol = IPPROTO_ICMP;
    ip_reply->src_ip = ip->dst_ip;
    ip_reply->dst_ip = ip->src_ip;
    ip_reply->checksum = 0;
    ip_reply->checksum =
        ip_checksum(ip_reply, sizeof(struct ip_header));

    interface_send(packet,
                   sizeof(struct ip_header) + len);
}

        buffer,
        len
    );
}

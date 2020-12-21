# WORK IN PROGRESS

## Dependencies

  - [JSCON](https://github.com/LucasMull/JSCON)
  - [libcurl](https://github.com/curl/curl)
  - [libuv](https://github.com/libuv/libuv)

## Discord API Roadmap

### [V8 Error Messages](https://discord.com/developers/docs/reference#error-messages)
### [Authentication](https://discord.com/developers/docs/reference#authentication)
    - [x] [Bot Token Authentication](https://discord.com/developers/docs/reference#authentication-example-bot-token-authorization-header)
    - [ ] [Bearer Token Authentication](https://discord.com/developers/docs/reference#authentication-example-bearer-token-authorization-header)
### [RateLimits](https://discord.com/developers/docs/topics/rate-limits)
    - [x] RateLimit Bucket Queuing
    - [x] [Exceeding A Rate Limit](https://discord.com/developers/docs/topics/rate-limits#exceeding-a-rate-limit)
    - [ ] [Invalid Request Limit](https://discord.com/developers/docs/topics/rate-limits#invalid-request-limit)
### [Slash Commands](https://discord.com/developers/docs/interactions/slash-commands#slash-commands)
### [Audit Log](https://discord.com/developers/docs/resources/audit-log#audit-logs-resource)
### [Channels Resource](https://discord.com/developers/docs/resources/channel#channels-resource)
    - [x] [Channel Object](https://discord.com/developers/docs/resources/channel#channel-object)
    - [ ] [Channel Types](https://discord.com/developers/docs/resources/channel#channel-object-channel-types)
    - [ ] [Message Object](https://discord.com/developers/docs/resources/channel#message-object)
    - [ ] [Followed Channel Object](https://discord.com/developers/docs/resources/channel#followed-channel-object)
    - [ ] [Reaction Object](https://discord.com/developers/docs/resources/channel#reaction-object)
    - [ ] [Overwrite Object](https://discord.com/developers/docs/resources/channel#overwrite-object)
    - [ ] [Embed Object](https://discord.com/developers/docs/resources/channel#embed-object)
    - [ ] [Attachment Object](https://discord.com/developers/docs/resources/channel#attachment-object)
    - [ ] [Channel Mention Object](https://discord.com/developers/docs/resources/channel#channel-mention-object)
    - [ ] [Allowed Mentions Object](https://discord.com/developers/docs/resources/channel#allowed-mentions-object)
    - [ ] [Embed Limits](https://discord.com/developers/docs/resources/channel#embed-limits)
    - [x] [Get Channel](https://discord.com/developers/docs/resources/channel#get-channel)
    - [ ] [Modify Channel](https://discord.com/developers/docs/resources/channel#modify-channel)
    - [ ] [Delete/Close Channel](https://discord.com/developers/docs/resources/channel#deleteclose-channel)
    - [ ] [Get Channel Messages](https://discord.com/developers/docs/resources/channel#get-channel-messages)
    - [ ] [Get Channel Message](https://discord.com/developers/docs/resources/channel#get-channel-message)
    - [ ] [Create Message](https://discord.com/developers/docs/resources/channel#create-message)
    - [ ] [Crosspost Message](https://discord.com/developers/docs/resources/channel#crosspost-message)
    - [ ] [Create Reaction](https://discord.com/developers/docs/resources/channel#create-reaction)
    - [ ] [Delete Own Reaction](https://discord.com/developers/docs/resources/channel#delete-own-reaction)
    - [ ] [Delete User Reaction](https://discord.com/developers/docs/resources/channel#delete-user-reaction)
    - [ ] [Get Reactions](https://discord.com/developers/docs/resources/channel#get-reactions)
    - [ ] [Delete All Reactions](https://discord.com/developers/docs/resources/channel#delete-all-reactions)
    - [ ] [Delete All Reactions for Emoji](https://discord.com/developers/docs/resources/channel#delete-all-reactions-for-emoji)
    - [ ] [Edit Message](https://discord.com/developers/docs/resources/channel#edit-message)
    - [ ] [Delete Message](https://discord.com/developers/docs/resources/channel#delete-message)
    - [ ] [Bulk Delete Messages](https://discord.com/developers/docs/resources/channel#bulk-delete-messages)
    - [ ] [Edit Channel Permissions](https://discord.com/developers/docs/resources/channel#edit-channel-permissions)
    - [ ] [Get Channel Invites](https://discord.com/developers/docs/resources/channel#get-channel-invites)
    - [ ] [Create Channel Invite](https://discord.com/developers/docs/resources/channel#create-channel-invite)
    - [ ] [Delete Channel Permission](https://discord.com/developers/docs/resources/channel#delete-channel-permission)
    - [ ] [Follow News Channels](https://discord.com/developers/docs/resources/channel#follow-news-channel)
    - [ ] [Trigger Typing Indicator](https://discord.com/developers/docs/resources/channel#trigger-typing-indicator)
    - [ ] [Get Pinned Messages](https://discord.com/developers/docs/resources/channel#get-pinned-messages)
    - [ ] [Add Pinned Channel Message](https://discord.com/developers/docs/resources/channel#add-pinned-channel-message)
    - [ ] [Delete Pinned Channel Message](https://discord.com/developers/docs/resources/channel#delete-pinned-channel-message)
    - [ ] [Group DM Add Recipient](https://discord.com/developers/docs/resources/channel#group-dm-add-recipient)
    - [ ] [Group DM Remove Recipient](https://discord.com/developers/docs/resources/channel#group-dm-remove-recipient)
### [Emoji Resource](https://discord.com/developers/docs/resources/emoji#emoji-resource)
### [Guild Resource](https://discord.com/developers/docs/resources/guild#guild-resource)
    - [x] [Guild Object](https://discord.com/developers/docs/resources/guild#guild-object)
    - [ ] [Guild Widget Object](https://discord.com/developers/docs/resources/guild#guild-widget-object)
    - [ ] [Guild Member Object](https://discord.com/developers/docs/resources/guild#guild-member-object)
    - [ ] [Integration Object](https://discord.com/developers/docs/resources/guild#integration-object)
    - [ ] [Ban Object](https://discord.com/developers/docs/resources/guild#ban-object)
    - [ ] [Create Guild](https://discord.com/developers/docs/resources/guild#create-guild)
    - [x] [Get Guild](https://discord.com/developers/docs/resources/guild#get-guild)
    - [ ] [Get Guild Preview](https://discord.com/developers/docs/resources/guild#get-guild-preview)
    - [ ] [Modify Guild](https://discord.com/developers/docs/resources/guild#modify-guild)
    - [ ] [Delete Guild](https://discord.com/developers/docs/resources/guild#delete-guild)
    - [x] [Get Guild Channels](https://discord.com/developers/docs/resources/guild#get-guild-channels)
    - [ ] [Get Guild Channel](https://discord.com/developers/docs/resources/guild#create-guild-channel)
    - [ ] [Modify Guild Channel Position](https://discord.com/developers/docs/resources/guild#modify-guild-channel-positions)
    - [ ] [Get Guild Member](https://discord.com/developers/docs/resources/guild#get-guild-member)
    - [ ] [List Guild Members](https://discord.com/developers/docs/resources/guild#list-guild-members)
    - [ ] [Add Guild Member](https://discord.com/developers/docs/resources/guild#add-guild-member)
    - [ ] [Modify Guild Member](https://discord.com/developers/docs/resources/guild#modify-guild-member)
    - [ ] [Modify Current User Nick](https://discord.com/developers/docs/resources/guild#modify-current-user-nick)
    - [ ] [Add Guild Member Role](https://discord.com/developers/docs/resources/guild#add-guild-member-role)
    - [ ] [Remove Guild Member Role](https://discord.com/developers/docs/resources/guild#remove-guild-member-role)
    - [ ] [Get Guild Bans](https://discord.com/developers/docs/resources/guild#get-guild-bans)
    - [ ] [Get Guild Ban](https://discord.com/developers/docs/resources/guild#get-guild-ban)
    - [ ] [Create Guild Ban](https://discord.com/developers/docs/resources/guild#create-guild-ban)
    - [ ] [Remove Guild Ban](https://discord.com/developers/docs/resources/guild#remove-guild-ban)
    - [ ] [Get Guild Roles](https://discord.com/developers/docs/resources/guild#get-guild-roles)
    - [ ] [Create Guild Role](https://discord.com/developers/docs/resources/guild#create-guild-role)
    - [ ] [Modify Guild Role Positions](https://discord.com/developers/docs/resources/guild#modify-guild-role-positions)
    - [ ] [Modify Guild Role](https://discord.com/developers/docs/resources/guild#modify-guild-role)
    - [ ] [Delete Guild Role](https://discord.com/developers/docs/resources/guild#delete-guild-role)
    - [ ] [Get Guild Prune Count](https://discord.com/developers/docs/resources/guild#get-guild-prune-count)
    - [ ] [Begin Guild Prune](https://discord.com/developers/docs/resources/guild#begin-guild-prune)
    - [ ] [Get Guild Voice Regions](https://discord.com/developers/docs/resources/guild#get-guild-voice-regions)
    - [ ] [Get Guild Invites](https://discord.com/developers/docs/resources/guild#get-guild-invites)
    - [ ] [Get Guild Integrations](https://discord.com/developers/docs/resources/guild#get-guild-integrations)
    - [ ] [Create Guild Integration](https://discord.com/developers/docs/resources/guild#create-guild-integration)
    - [ ] [Modify Guild Integration](https://discord.com/developers/docs/resources/guild#modify-guild-integration)
    - [ ] [Delete Guild Integration](https://discord.com/developers/docs/resources/guild#delete-guild-integration)
    - [ ] [Sync Guild Integration](https://discord.com/developers/docs/resources/guild#sync-guild-integration)
    - [ ] [Get Guild Widget Settings](https://discord.com/developers/docs/resources/guild#get-guild-widget-settings)
    - [ ] [Modify Guild Widget](https://discord.com/developers/docs/resources/guild#modify-guild-widget)
    - [ ] [Get Guild Widget](https://discord.com/developers/docs/resources/guild#get-guild-widget)
    - [ ] [Get Guild Vanity URL](https://discord.com/developers/docs/resources/guild#get-guild-vanity-url)
    - [ ] [Get Guild Widget Image](https://discord.com/developers/docs/resources/guild#get-guild-widget-image)
### [Invite Resource](https://discord.com/developers/docs/resources/invite#invite-resource)
### [Template Resource](https://discord.com/developers/docs/resources/template#template-resource)
### [User Resource](https://discord.com/developers/docs/resources/user#users-resource)
    - [ ] [User Object](https://discord.com/developers/docs/resources/user#user-object)
    - [ ] [Premium Types](https://discord.com/developers/docs/resources/user#user-object-premium-types)
    - [ ] [Connection Object](https://discord.com/developers/docs/resources/user#connection-object)
    - [x] [Get Current User](https://discord.com/developers/docs/resources/user#get-current-user)
    - [x] [Get User](https://discord.com/developers/docs/resources/user#get-user)
    - [ ] [Modify Current User](https://discord.com/developers/docs/resources/user#modify-current-user)
    - [x] [Get Current User Guilds](https://discord.com/developers/docs/resources/user#get-current-user-guilds)
    - [ ] [Leave Guild](https://discord.com/developers/docs/resources/user#leave-guild)
    - [ ] [Get User DMs](https://discord.com/developers/docs/resources/user#get-user-dms)
    - [ ] [Create DM](https://discord.com/developers/docs/resources/user#create-dm)
    - [ ] [Create Group DM](https://discord.com/developers/docs/resources/user#create-group-dm)
    - [ ] [Get User Connections](https://discord.com/developers/docs/resources/user#get-user-connections)
### [Voice Resouce](https://discord.com/developers/docs/resources/voice#voice-resource)
### [Webhook Resource](https://discord.com/developers/docs/resources/webhook#webhook-resource)
### [Gateway](https://discord.com/developers/docs/topics/gateway)
    - [x] [Payloads](https://discord.com/developers/docs/topics/gateway#payloads)
    - [ ] [Encoding and Compression](https://discord.com/developers/docs/topics/gateway#encoding-and-compression)
    - [x] [Connecting to the Gateway](https://discord.com/developers/docs/topics/gateway#connecting-to-the-gateway)
    - [ ] [Resuming](https://discord.com/developers/docs/topics/gateway#resuming)
    - [x] [Disconnections](https://discord.com/developers/docs/topics/gateway#disconnections)
    - [ ] [Gateway Intents](https://discord.com/developers/docs/topics/gateway#gateway-intents)
    - [ ] [Rate Limiting](https://discord.com/developers/docs/topics/gateway#rate-limiting)
    - [ ] [Sharding](https://discord.com/developers/docs/topics/gateway#sharding)
    - [ ] [Commands and Events](https://discord.com/developers/docs/topics/gateway#commands-and-events)
    - [ ] [Channels](https://discord.com/developers/docs/topics/gateway#channels)
    - [ ] [Guilds](https://discord.com/developers/docs/topics/gateway#guilds)
    - [ ] [Invites](https://discord.com/developers/docs/topics/gateway#invites)
    - [ ] [Messages](https://discord.com/developers/docs/topics/gateway#messages)
    - [ ] [Presence](https://discord.com/developers/docs/topics/gateway#presence)
    - [ ] [Voice](https://discord.com/developers/docs/topics/gateway#voice)
    - [ ] [Webhooks](https://discord.com/developers/docs/topics/gateway#webhooks)
    - [ ] [Interactions](https://discord.com/developers/docs/topics/gateway#interactions)
    - [ ] [Get Gateway](https://discord.com/developers/docs/topics/gateway#get-gateway)
    - [ ] [Get Gateway Bot](https://discord.com/developers/docs/topics/gateway#get-gateway-bot)

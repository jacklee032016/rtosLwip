/*
* initialized all NMOS objects 
*/

/*
* basic operation of Node API
*/


#include "lwipMux.h"

#include "muxHttp.h"
#include "jsmn.h"

#define	MUX_AUDIO_SAMPLE_RATE		48000

#define	MUX_VIDEO_FRAME_RATE			60
#define	MUX_VIDEO_FRAME_WIDTH		1080
#define	MUX_VIDEO_FRAME_HEIGHT		720



static const char	*_interfaceName = "MuxEthernet";


/***********************   init SOURCEs/FLOWs/SENDERs object     **********************/
static MuxNmosSender		_senderVideo, _senderAudio, _senderData;
static MuxNmosFlow		_flowVideo,_flowAudio, _flowData;




static MuxNmosSource		_sourceVideo;

static MuxNmosSource		_sourceAudio;

static MuxNmosSource		_sourceData;

static unsigned char __muxDataSenderInit(MuxNmosFlow *flow)
{
	MuxNmosSender *snd = &_senderData;
	memset(snd, 0, sizeof(MuxNmosSender));

	muxNmosIdGenerate(&snd->resourceId.nmosId, flow->source->device->node->runCfg);
	snprintf(snd->resourceId.label, sizeof(snd->resourceId.label), "MuxLab data sender from %s", flow->source->device->node->runCfg->name );
	snprintf(snd->resourceId.description, sizeof(snd->resourceId.description), "%s", "MuxLab NMOS data sender");
	snd->resourceId.node = flow->resourceId.node;

	ADD_ELEMENT( flow->source->device->senders, snd);
	snd->device = flow->source->device;
	snd->flow = flow;
	snd->enabled = 1;

	snd->transport = NMOS_TRANS_T_UCAST;
	snd->interface_binding = _interfaceName;

	snd->constraints.parent = &snd->resourceId;

	snd->constraints.sourcePortMini = flow->resourceId.node->runCfg->local.dport;
	snd->constraints.destPortMini = flow->resourceId.node->runCfg->dest.dport;
	snd->constraints.portRange = MUX_MEDIA_PORT_RANGE;

	return EXIT_SUCCESS;
}


static unsigned char __muxDataFlowInit(MuxNmosSource	*src)
{
	MuxNmosFlow *flow = &_flowData;
	memset(flow, 0, sizeof(MuxNmosFlow));

	muxNmosIdGenerate(&flow->resourceId.nmosId, src->device->node->runCfg);
	snprintf(flow->resourceId.label, sizeof(flow->resourceId.label), "MuxLab data from %s", src->device->node->runCfg->name );
	snprintf(flow->resourceId.description, sizeof(flow->resourceId.description), "%s", "MuxLab data supports NMOS");
	flow->resourceId.node = src->resourceId.node;

	flow->source = src;
	src->flow = flow;
	flow->format = NMOS_SOURCE_FORMAT_DATA;
	snprintf(flow->mediaType, sizeof(flow->mediaType), "%s", NMOS_MEDIA_TYPE_DATA_291);
	
	__muxDataSenderInit(flow);
	
	ADD_ELEMENT(src->device->flows, flow);
	return EXIT_SUCCESS;
}


static unsigned char __muxDataSourceInit(MuxNmosDevice *dev)
{
	MuxNmosSource	*src = &_sourceData;

	memset(src, 0, sizeof(MuxNmosSource));
	muxNmosIdGenerate(&src->resourceId.nmosId, dev->node->runCfg);
	snprintf(src->resourceId.label, sizeof(src->resourceId.label), "MuxLab data stream from %s", dev->node->runCfg->name );
	snprintf(src->resourceId.description, sizeof(src->resourceId.description), "%s", "MuxLab data stream supports NMOS");
	src->resourceId.node = dev->resourceId.node;

	/* source core fields */
	src->device = dev;
	src->clockIndex = NMOS_CLOCK_INTERNAL;
	src->format = NMOS_SOURCE_FORMAT_DATA;

	__muxDataFlowInit(src);

	ADD_ELEMENT(dev->sources, src);
	return EXIT_SUCCESS;
}

static NmosAudioChannel		_leftChannel;
static NmosAudioChannel		_rightChannel;

static void	_muxNmosAudioChannelInit(MuxNmosSource *audioSrc)
{
	NmosAudioChannel *chan = &_leftChannel;
	
	audioSrc->audioChannels = chan;
	memset(chan, 0, sizeof(NmosAudioChannel) );
	chan->symbol = NMOS_AUDIO_CHAN_T_L;
	snprintf(chan->label, sizeof(chan->label), "%s", "Left Channel");

	chan->next = &_rightChannel;
	chan = chan->next;

	memset(chan, 0, sizeof(NmosAudioChannel) );
	chan->symbol = NMOS_AUDIO_CHAN_T_R;
	snprintf(chan->label, sizeof(chan->label), "%s", "Right Channel");

}

static unsigned char __muxAudioSenderInit(MuxNmosFlow *flow)
{
	MuxNmosSender *snd = &_senderAudio;
	memset(snd, 0, sizeof(MuxNmosSender));

	muxNmosIdGenerate(&snd->resourceId.nmosId, flow->source->device->node->runCfg);
	snprintf(snd->resourceId.label, sizeof(snd->resourceId.label), "MuxLab audio sender from %s", flow->source->device->node->runCfg->name );
	snprintf(snd->resourceId.description, sizeof(snd->resourceId.description), "%s", "MuxLab NMOS audio sender");
	snd->resourceId.node = flow->resourceId.node;

	ADD_ELEMENT( flow->source->device->senders, snd);
	snd->device = flow->source->device;
	snd->flow = flow;
	snd->enabled = 1;

	snd->transport = NMOS_TRANS_T_UCAST;
	snd->interface_binding = _interfaceName;

	snd->constraints.parent = &snd->resourceId;

	snd->constraints.sourcePortMini = flow->resourceId.node->runCfg->local.aport;
	snd->constraints.destPortMini = flow->resourceId.node->runCfg->dest.aport;
	snd->constraints.portRange = MUX_MEDIA_PORT_RANGE;
	
	return EXIT_SUCCESS;
}


static unsigned char __muxAudioFlowInit(MuxNmosSource	*src)
{
	MuxNmosFlow *flow = &_flowAudio;
	memset(flow, 0, sizeof(MuxNmosFlow));

	muxNmosIdGenerate(&flow->resourceId.nmosId, src->device->node->runCfg);
	snprintf(flow->resourceId.label, sizeof(flow->resourceId.label), "MuxLab audio from %s", src->device->node->runCfg->name );
	snprintf(flow->resourceId.description, sizeof(flow->resourceId.description), "%s", "MuxLab audio supports NMOS");
	flow->resourceId.node = src->resourceId.node;

	flow->source = src;
	src->flow = flow;
	flow->format = NMOS_SOURCE_FORMAT_AUDIO;
	snprintf(flow->mediaType, sizeof(flow->mediaType), "%s", NMOS_MEDIA_TYPE_AUDIO_L16);
	
	flow->sampleRate.denominator = 1;
	flow->sampleRate.numberator = MUX_AUDIO_SAMPLE_RATE;
	flow->bitDepth = 16;

	__muxAudioSenderInit(flow);
	
	ADD_ELEMENT(src->device->flows, flow);
	return EXIT_SUCCESS;
}

static unsigned char __muxAudioSourceInit(MuxNmosDevice *dev)
{
	MuxNmosSource	*src = &_sourceAudio;
	memset(src, 0, sizeof(MuxNmosSource));
	memset(src, 0, sizeof(MuxNmosSource));

	/* source core fields */
	muxNmosIdGenerate(&src->resourceId.nmosId, dev->node->runCfg);
	snprintf(src->resourceId.label, sizeof(src->resourceId.label), "MuxLab audio stream from %s", dev->node->runCfg->name );
	snprintf(src->resourceId.description, sizeof(src->resourceId.description), "%s", "MuxLab audio stream supports NMOS");
	src->resourceId.node = dev->resourceId.node;

	src->device = dev;
	src->clockIndex = NMOS_CLOCK_INTERNAL;
	src->format = NMOS_SOURCE_FORMAT_AUDIO;

	_muxNmosAudioChannelInit(src);

	src->gRate.denominator = 1;
	src->gRate.numberator = MUX_VIDEO_FRAME_RATE;

	__muxAudioFlowInit(src);

	ADD_ELEMENT(dev->sources, src);

	return EXIT_SUCCESS;
}


static unsigned char __muxVideoSenderInit(MuxNmosFlow *flow)
{
	MuxNmosSender *snd = &_senderVideo;
	memset(snd, 0, sizeof(MuxNmosSender));

	muxNmosIdGenerate(&snd->resourceId.nmosId, flow->source->device->node->runCfg);
	snprintf(snd->resourceId.label, sizeof(snd->resourceId.label), "MuxLab video sender from %s", flow->source->device->node->runCfg->name );
	snprintf(snd->resourceId.description, sizeof(snd->resourceId.description), "%s", "MuxLab NMOS video sender");
	snd->resourceId.node = flow->resourceId.node;

	ADD_ELEMENT( flow->source->device->senders, snd);
	snd->device = flow->source->device;
	snd->flow = flow;
	snd->enabled = 1;

	snd->transport = NMOS_TRANS_T_UCAST;
	snd->interface_binding = _interfaceName;

	snd->constraints.parent = &snd->resourceId;
	snd->constraints.sourcePortMini = flow->resourceId.node->runCfg->local.vport;
	snd->constraints.destPortMini = flow->resourceId.node->runCfg->dest.vport;
	snd->constraints.portRange = MUX_MEDIA_PORT_RANGE;

	return EXIT_SUCCESS;
}


static const NmosVideoComponent _crComponent = 
{
	name	:	NMOS_VIDEO_CMP_TYPE_T_CR,
	width	:	960,
	height	:	1080,
	bitDepth	:	10,

	 next	:	NULL
};

static const NmosVideoComponent _cyComponent = 
{
	name	:	NMOS_VIDEO_CMP_TYPE_T_CB,
	width	:	960,
	height	:	1080,
	bitDepth	:	10,

	 next	:	&_crComponent
};



static NmosVideoComponent _yComponent = 
{
	name	:	NMOS_VIDEO_CMP_TYPE_T_Y,
	width	:	1920,
	height	:	1080,
	bitDepth	:	10,

	 next	:	&_cyComponent
};


static unsigned char __muxVideoFlowInit(MuxNmosSource	*src)
{
	MuxNmosFlow *flow = &_flowVideo;
	memset(flow, 0, sizeof(MuxNmosFlow));

	muxNmosIdGenerate(&flow->resourceId.nmosId, src->device->node->runCfg);
	snprintf(flow->resourceId.label, sizeof(flow->resourceId.label), "MuxLab raw video from %s", src->device->node->runCfg->name );
	snprintf(flow->resourceId.description, sizeof(flow->resourceId.description), "%s", "MuxLab raw video supports NMOS");
	flow->resourceId.node = src->resourceId.node;

	flow->source = src;
	src->flow = flow;
	flow->format = NMOS_SOURCE_FORMAT_VIDEO;
	snprintf(flow->mediaType, sizeof(flow->mediaType), "%s", NMOS_MEDIA_TYPE_VIDEO_RAW);
	
	flow->colorspace = NMOS_VIDEO_CS_T_BT2100;
	flow->frameWidth = MUX_VIDEO_FRAME_WIDTH;
	flow->frameHeight = MUX_VIDEO_FRAME_HEIGHT;

	flow->interlaceMode= NMOS_VIDEO_ILM_T_PROGRESSIVE;
	flow->transferCharacter = NMOS_VIDEO_TC_T_SDR;
	flow->components = &_yComponent;

	__muxVideoSenderInit(flow);

	ADD_ELEMENT(src->device->flows, flow);

	return EXIT_SUCCESS;
}

static unsigned char __muxVideoSourceInit(MuxNmosDevice *dev)
{
	MuxNmosSource	*src = &_sourceVideo;
	memset(src, 0, sizeof(MuxNmosSource));

	muxNmosIdGenerate(&src->resourceId.nmosId, dev->node->runCfg);
	snprintf(src->resourceId.label, sizeof(src->resourceId.label), "MuxLab raw video from %s", dev->node->runCfg->name );
	snprintf(src->resourceId.description, sizeof(src->resourceId.description), "%s", "MuxLab raw video supports NMOS");
	src->resourceId.node = dev->resourceId.node;
	
	/* source core fields */
	src->device = dev;
	src->clockIndex = NMOS_CLOCK_INTERNAL;
	src->format = NMOS_SOURCE_FORMAT_VIDEO;

	src->gRate.denominator = 1;
	src->gRate.numberator = MUX_VIDEO_FRAME_RATE;

	ADD_ELEMENT(dev->sources, src);

	__muxVideoFlowInit(src);
	
	return EXIT_SUCCESS;
}

/* only for TX */
static void	_muxNmosSourceInit(MuxNmosDevice *dev)
{
	unsigned char ret;
	/* video */
	ret = __muxVideoSourceInit(dev);
	MUX_ASSERT(("Video Source initialization failed"), ret==EXIT_SUCCESS);

	ret = __muxAudioSourceInit(dev);
	MUX_ASSERT(("Audio Source initialization failed"), ret==EXIT_SUCCESS);

	ret = __muxDataSourceInit(dev);
	MUX_ASSERT(("Data Source initialization failed"), ret==EXIT_SUCCESS);

	ret = ret;
}


/******** For RX, initialize all RECEIVERs ********************/

static MuxNmosReceiver	_receiverVideo, _receiverAudio, _receiverData;

static const NmosMediaType _recvMediaVideo = {NMOS_MEDIA_TYPE_VIDEO_RAW, NULL};
static const NmosMediaType _recvMediaAudio = {NMOS_MEDIA_TYPE_AUDIO_L16, NULL};
static const NmosMediaType _recvMediaData = {NMOS_MEDIA_TYPE_DATA_291,  NULL};

static unsigned char _muxNmosReceiversInit(MuxNmosDevice *dev)
{
	/* video receiver */
	MuxNmosReceiver	*rcv = &_receiverVideo;
	memset(rcv, 0, sizeof(MuxNmosReceiver));

	snprintf(rcv->resourceId.label, sizeof(rcv->resourceId.label), "MuxLab video receiver from %s", dev->node->runCfg->name );
	snprintf(rcv->resourceId.description, sizeof(rcv->resourceId.description), "%s", "MuxLab NMOS video receiver");
	rcv->resourceId.node = dev->resourceId.node;
	muxNmosIdGenerate(&rcv->resourceId.nmosId, dev->node->runCfg);
	rcv->device = dev;

	rcv->interface_binding = _interfaceName;
	rcv->format = NMOS_SOURCE_FORMAT_VIDEO;
	rcv->caps = &_recvMediaVideo;
	rcv->constraints.parent = &rcv->resourceId;

	ADD_ELEMENT(dev->receivers, rcv);

	/* audio receiver */
	rcv = &_receiverAudio;
	memset(rcv, 0, sizeof(MuxNmosReceiver));

	snprintf(rcv->resourceId.label, sizeof(rcv->resourceId.label), "MuxLab audio receiver from %s", dev->node->runCfg->name );
	snprintf(rcv->resourceId.description, sizeof(rcv->resourceId.description), "%s", "MuxLab NMOS audio receiver");
	rcv->resourceId.node = dev->resourceId.node;
	muxNmosIdGenerate(&rcv->resourceId.nmosId, dev->node->runCfg);
	rcv->device = dev;

	rcv->interface_binding = _interfaceName;
	rcv->format = NMOS_SOURCE_FORMAT_AUDIO;
	rcv->caps = &_recvMediaAudio;
	rcv->constraints.parent = &rcv->resourceId;

	ADD_ELEMENT(dev->receivers, rcv);

	/* data receiver */
	rcv = &_receiverData;
	memset(rcv, 0, sizeof(MuxNmosReceiver));

	snprintf(rcv->resourceId.label, sizeof(rcv->resourceId.label), "MuxLab data receiver from %s", dev->node->runCfg->name );
	snprintf(rcv->resourceId.description, sizeof(rcv->resourceId.description), "%s", "MuxLab NMOS data receiver");
	rcv->resourceId.node = dev->resourceId.node;
	muxNmosIdGenerate(&rcv->resourceId.nmosId, dev->node->runCfg);
	rcv->device = dev;

	rcv->interface_binding = _interfaceName;
	rcv->format = NMOS_SOURCE_FORMAT_DATA;
	rcv->caps = &_recvMediaData;
	rcv->constraints.parent = &rcv->resourceId;

	ADD_ELEMENT(dev->receivers, rcv);

	return EXIT_SUCCESS;
}

/** init Node and Device Object */
static MuxNmosDevice		_nmosDevice;
MuxNmosNode		nmosNode;

void	muxNmosNodeInit(MuxNmosNode	*node, MUX_RUNTIME_CFG *runCfg)
{
	MuxNmosDevice *dev = &_nmosDevice;

	memset(dev, 0, sizeof(MuxNmosDevice));
	
	memset(node, 0, sizeof(MuxNmosNode));
	snprintf(node->resourceId.label, sizeof(node->resourceId.label), "%s", "Label of MuxLab NMOS node");
	snprintf(node->resourceId.description, sizeof(node->resourceId.description), "%s", "Description of MuxLab NMOS node");
	node->resourceId.node = node;

	snprintf(dev->resourceId.label, sizeof(dev->resourceId.label), "%s", "Label of MuxLab NMOS device");
	snprintf(dev->resourceId.description, sizeof(dev->resourceId.description), "%s", "Description of MuxLab NMOS device");
	dev->resourceId.node = node;

	node->runCfg = runCfg;

	/* should save in NV RAM */
	muxNmosIdGenerate(&node->resourceId.nmosId, node->runCfg);
	muxNmosIdGenerate(&dev->resourceId.nmosId, node->runCfg);


	node->device = dev;
	dev->node = node;
	
	if(MUX_IS_TX(runCfg))
	{
		/* initialized SENDER */
		_muxNmosSourceInit(dev);
	}
	else
	{
		_muxNmosReceiversInit(dev);
	}

}



#ifndef TCPPACKETMODEL_H
#define TCPPACKETMODEL_H

#include <QAbstractItemModel>
#include "qpcaptcppacket.h"
#include "qpcaptcpconversation.h"
#include <QIcon>

class TcpPacketModel : public QAbstractItemModel {
  Q_OBJECT
private:
  class TreeItem {
  public:
    TreeItem *_parent;
    QPcapTcpPacket _packet;
    QPcapTcpConversation _conversation;
    QList<TreeItem *> _children;
    /** Means upstream for packets (if _conversation.isNull()) and finished
      * for conversations.
      */
    bool _upstreamFinished;

    TreeItem(TreeItem *parent, QPcapTcpPacket packet,
             QPcapTcpConversation conversation, bool upstream)
      : _parent(parent), _packet(packet), _conversation(conversation),
        _upstreamFinished(upstream) { }
    ~TreeItem() {
      qDeleteAll(_children);
    }
    int row() /*const*/ {
      return _parent ? _parent->_children.indexOf(this) : 0;
    }

  private:
    Q_DISABLE_COPY(TreeItem)
  };

  TreeItem* _root;
  QIcon _upstreamIcon, _downstreamIcon;

public:
  explicit TcpPacketModel(QObject *parent = 0);
  ~TcpPacketModel();
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QModelIndex index(QPcapTcpConversation) const;
  QModelIndex index(QPcapTcpPacket) const;
  /** @return packet pointed by index or null packet (isNull()) if index points
    * to a conversation
    */
  QPcapTcpPacket packet(const QModelIndex &index) const;
  /** @return conversation pointed by index, or conversation to which packet
    * pointed by index belong to.
    */
  QPcapTcpConversation conversation(const QModelIndex &index) const;
  void clear();

signals:
  
public slots:
  void addTcpUpstreamPacket(QPcapTcpPacket packet,
                            QPcapTcpConversation conversation);
  void addTcpDownstreamPacket(QPcapTcpPacket packet,
                              QPcapTcpConversation conversation);

private:
  void addPacket(QPcapTcpConversation conversation, QPcapTcpPacket packet,
                 bool upstream);
};

#endif // TCPPACKETMODEL_H
